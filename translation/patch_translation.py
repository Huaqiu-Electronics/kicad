#!/usr/bin/env python3
import re
import sys
import json
from pathlib import Path
import polib


def extract_translations_from_git_patch(patch_path: Path):
    """Extract updated msgid/msgstr pairs from a git diff patch."""
    lines = patch_path.read_text(encoding="utf-8", errors="ignore").splitlines()

    translations = {}
    current_msgid = None
    old_msgstr = None
    new_msgstr = None

    for i, line in enumerate(lines):
        # Capture msgid from context
        if re.match(r'^[ @#]*msgid ', line):
            m = re.search(r'"(.*)"', line)
            if m:
                current_msgid = m.group(1)

        # Detect changed msgstr lines
        if line.startswith("-msgstr "):
            old_msgstr = re.findall(r'"(.*)"', line)
            old_msgstr = old_msgstr[0] if old_msgstr else ""
        elif line.startswith("+msgstr "):
            new_msgstr = re.findall(r'"(.*)"', line)
            new_msgstr = new_msgstr[0] if new_msgstr else ""
            if current_msgid and new_msgstr is not None:
                translations[current_msgid] = new_msgstr
                old_msgstr = new_msgstr = None

    return translations


def update_po_with_translations(po_path: Path, updates: dict):
    po = polib.pofile(po_path)
    updated_count = 0

    for entry in po:
        if entry.msgid in updates:
            new_str = updates[entry.msgid]
            if new_str and entry.msgstr != new_str:
                entry.msgstr = new_str
                updated_count += 1

    backup_path = po_path.with_suffix(".bak.po")
    print(f"[*] Backing up {po_path} -> {backup_path}")
    po.save(backup_path)
    po.save(po_path)
    print(f"[+] Updated {updated_count} translations in {po_path}")


def main():
    if len(sys.argv) != 3:
        print("Usage: python patch_translation.py <path/to/file.po> <path/to/patch.diff>")
        sys.exit(1)

    po_path = Path(sys.argv[1])
    patch_path = Path(sys.argv[2])
    json_path = patch_path.with_suffix(".json")

    if not po_path.exists() or not patch_path.exists():
        print("[!] File not found.")
        sys.exit(1)

    print("[*] Extracting updated translations from patch...")
    updates = extract_translations_from_git_patch(patch_path)
    print(f"[+] Extracted {len(updates)} updated translations.")

    # Save JSON for reuse
    with open(json_path, "w", encoding="utf-8") as f:
        json.dump(updates, f, ensure_ascii=False, indent=2)
    print(f"[+] Saved extracted translations to {json_path}")

    update_po_with_translations(po_path, updates)


if __name__ == "__main__":
    main()
