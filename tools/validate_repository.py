#!/usr/bin/env python3
"""Validate project references, documentation and tracked ARMCC artifacts."""

from __future__ import annotations

import hashlib
import re
import struct
import sys
import xml.etree.ElementTree as ET
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
PROJECT = ROOT / "Projects/USER/Project.uvprojx"
ERRORS: list[str] = []


def require(condition: bool, message: str) -> None:
    if not condition:
        ERRORS.append(message)


def read(relative: str) -> str:
    path = ROOT / relative
    require(path.is_file(), f"missing file: {relative}")
    return path.read_text(encoding="utf-8-sig", errors="replace") if path.is_file() else ""


def local_path(base: Path, value: str) -> Path:
    return (base / value.replace("\\", "/")).resolve()


def validate_links(relative: str) -> None:
    for target in re.findall(r"\[[^\]]*\]\(([^)]+)\)", read(relative)):
        target = target.split("#", 1)[0]
        if not target or "://" in target or target.startswith("mailto:"):
            continue
        require(local_path((ROOT / relative).parent, target).exists(),
                f"broken local link in {relative}: {target}")


def parse_ihex(path: Path) -> tuple[dict[int, int], int | None]:
    memory: dict[int, int] = {}
    upper = 0
    start = None
    saw_eof = False
    for number, line in enumerate(path.read_text(encoding="ascii").splitlines(), 1):
        require(line.startswith(":"), f"HEX line {number} has no colon")
        if not line.startswith(":"):
            continue
        try:
            record = bytes.fromhex(line[1:])
        except ValueError:
            require(False, f"HEX line {number} is not hexadecimal")
            continue
        require(len(record) >= 5 and len(record) == record[0] + 5,
                f"HEX line {number} has invalid length")
        require(sum(record) & 0xFF == 0, f"HEX line {number} checksum failed")
        if len(record) < 5:
            continue
        count = record[0]
        address = int.from_bytes(record[1:3], "big")
        kind = record[3]
        data = record[4:4 + count]
        if kind == 0:
            for offset, value in enumerate(data):
                memory[upper + address + offset] = value
        elif kind == 1:
            saw_eof = True
        elif kind == 4 and len(data) == 2:
            upper = int.from_bytes(data, "big") << 16
        elif kind == 5 and len(data) == 4:
            start = int.from_bytes(data, "big")
    require(saw_eof, "HEX end-of-file record is missing")
    return memory, start


def parse_elf_load(path: Path) -> tuple[int, int, int, bytes]:
    blob = path.read_bytes()
    require(blob[:4] == b"\x7fELF", "AXF is not ELF")
    require(blob[4:6] == b"\x01\x01", "AXF is not ELF32 little-endian")
    header = struct.unpack_from("<16sHHIIIIIHHHHHH", blob, 0)
    machine, entry, phoff = header[2], header[4], header[5]
    phentsize, phnum = header[9], header[10]
    require(machine == 40, f"AXF machine is {machine}, expected ARM (40)")
    loads: list[tuple[int, int, int, bytes]] = []
    for index in range(phnum):
        fields = struct.unpack_from("<IIIIIIII", blob, phoff + index * phentsize)
        kind, offset, _vaddr, paddr, filesz, memsz, _flags, _align = fields
        if kind == 1:
            loads.append((paddr, filesz, memsz, blob[offset:offset + filesz]))
    require(len(loads) == 1, f"AXF has {len(loads)} load segments, expected 1")
    paddr, filesz, memsz, payload = loads[0] if loads else (0, 0, 0, b"")
    return entry, paddr, memsz, payload


tree = ET.parse(PROJECT)
root = tree.getroot()
device = root.findtext(".//Device") or ""
cpu = root.findtext(".//Cpu") or ""
compiler = root.findtext(".//pCCUsed") or ""
pack = root.findtext(".//PackID") or ""
require(device == "STM32F103T8", f"unexpected Keil target: {device}")
require("IROM(0x08000000,0x00010000)" in cpu, "project Flash is not 64 KiB")
require("IRAM(0x20000000,0x00005000)" in cpu, "project RAM is not 20 KiB")
require("ARMCC" in compiler and "5.06 update 6" in compiler, "unexpected compiler declaration")
require(pack == "Keil.STM32F1xx_DFP.2.4.1", f"unexpected STM32 pack: {pack}")

file_nodes = root.findall(".//FilePath")
require(len(file_nodes) == 50, f"project has {len(file_nodes)} file references, expected 50")
for node in file_nodes:
    value = node.text or ""
    require(local_path(PROJECT.parent, value).is_file(), f"missing project file: {value}")

include_text = root.findtext(".//Cads/VariousControls/IncludePath") or ""
includes = [item for item in include_text.split(";") if item]
for value in includes:
    require(local_path(PROJECT.parent, value).is_dir(), f"missing include directory: {value}")
require("../LIB/inc" not in includes and "../CMSIS" not in includes,
        "stale local include directories remain")

libraries = ["aitcmd.lib", "os.lib", "pdoa.lib", "twr.lib"]
for name in libraries:
    require((ROOT / "Components/HAL/DW/twr_pdoa/lib" / name).is_file(),
            f"missing precompiled library: {name}")

main = read("Components/Main/main.c")
defines = read("Components/Examples/examples/examples_info/examples_defines.h")
dispatcher = read("Components/Examples/examples/examples_info/example_info.c")
for symbol in ["main", "init", "nt_task", "node_start", "tag_start",
               "ds_twr_sts_sdc_responder", "ds_twr_sts_sdc_initiator"]:
    require(symbol in main, f"missing normal execution symbol: {symbol}")
examples = ["read_dev_id", "simple_tx", "tx_timed_sleep", "simple_tx_pdoa",
            "simple_rx", "simple_rx_pdoa", "tx_wait_resp", "rx_send_resp",
            "tx_wait_resp_int", "ds_twr_sts_sdc_init", "ds_twr_sts_sdc_resp"]
for symbol in examples:
    require(symbol in defines and symbol in dispatcher,
            f"example is not declared and dispatched: {symbol}")

pairs = [
    ("README.md", "README.zh.md"),
    ("docs/CODE_ENTRY.md", "docs/CODE_ENTRY.zh.md"),
    ("docs/ARCHITECTURE.md", "docs/ARCHITECTURE.zh.md"),
    ("docs/VALIDATION.md", "docs/VALIDATION.zh.md"),
]
for english, chinese in pairs:
    require("中文" in read(english) and Path(chinese).name in read(english),
            f"missing Chinese navigation: {english}")
    require("English" in read(chinese) and Path(english).name in read(chinese),
            f"missing English navigation: {chinese}")
    validate_links(english)
    validate_links(chinese)

axf = ROOT / "Projects/USER/Output/Project.axf"
hex_file = ROOT / "Projects/USER/Output/Project.hex"
expected = {
    axf: "3176233a9523ad799439f1ca97fe51c36d115f17d7a2c33ebf9c2525660156a4",
    hex_file: "575a2c3f59cd040a71a709e8f73977978eb10f6b126b297cfb5a779b6211a1b1",
}
for path, digest in expected.items():
    require(hashlib.sha256(path.read_bytes()).hexdigest() == digest,
            f"historical artifact hash changed: {path.relative_to(ROOT)}")

entry, load_address, memory_size, payload = parse_elf_load(axf)
hex_memory, hex_entry = parse_ihex(hex_file)
require(entry == 0x080000ED and hex_entry == entry, "AXF/HEX entry point mismatch")
require(load_address == 0x08000000, f"unexpected AXF load address: 0x{load_address:08x}")
require(len(payload) == 70164 and memory_size == 84900, "unexpected AXF load size")
require(len(hex_memory) == len(payload), "AXF/HEX payload lengths differ")
for offset, value in enumerate(payload):
    if hex_memory.get(load_address + offset) != value:
        require(False, f"AXF/HEX payload mismatch at 0x{load_address + offset:08x}")
        break

log = (ROOT / "Projects/USER/Output/Project.build_log.htm").read_bytes()
require(b"Program Size: Code=64868 RO-data=4304 RW-data=5084 ZI-data=10644" in log,
        "historical build size line changed")
require(b"0 Error(s), 0 Warning(s)" in log, "historical build result is not clean")

if ERRORS:
    print("Repository validation failed:")
    for error in ERRORS:
        print(f"- {error}")
    sys.exit(1)

print("Repository validation passed: project, entries, bilingual docs, and historical AXF/HEX evidence.")
print("Historical artifact evidence only; no fresh ARMCC build was performed.")
