#!/usr/bin/env python3
import sys
import json
from pathlib import Path
import polib


def load_translations_from_json(json_path: Path) -> dict:
    """Load msgid -> msgstr mappings from JSON."""
    with open(json_path, "r", encoding="utf-8") as f:
        data = json.load(f)

    if not isinstance(data, dict):
        raise ValueError("JSON must be an object mapping msgid to msgstr")

    return data


def update_po_with_translations(po_path: Path, updates: dict):
    po = polib.pofile(po_path)
    updated_count = 0

    for entry in po:
        if entry.msgid in updates:
            new_str = updates[entry.msgid]
            if new_str and entry.msgstr != new_str:
                entry.msgstr = new_str
                entry.flags = []
                entry.previous_msgid = None
                updated_count += 1

    backup_path = po_path.with_suffix(".bak.po")
    print(f"[*] Backing up {po_path} -> {backup_path}")
    po.save(backup_path)
    po.save(po_path)

    print(f"[+] Updated {updated_count} translations in {po_path}")


def main():
    if len(sys.argv) < 3:
        print("Usage: python json_to_po.py <translations.json> <file1.po> [file2.po ...]")
        sys.exit(1)

    json_path = Path(sys.argv[1])
    po_paths = [Path(p) for p in sys.argv[2:]]

    if not json_path.exists():
        print(f"[!] JSON file not found: {json_path}")
        sys.exit(1)

    for po_path in po_paths:
        if not po_path.exists():
            print(f"[!] PO file not found: {po_path}")
            sys.exit(1)

    print(f"[*] Loading translations from {json_path}")
    updates = load_translations_from_json(json_path)
    print(f"[+] Loaded {len(updates)} translations")

    for po_path in po_paths:
        update_po_with_translations(po_path, updates)


if __name__ == "__main__":
    main()
