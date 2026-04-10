# HQ Distribution

## Update Translations

### Preparation

- PO EDIT : https://poedit.net/
- gettext for Windows : https://www.gnu.org/software/gettext/

### Apply the HQ changes

1. Update the kicad.pot file

```bash
./translation/update-po-files.sh 
```

2. Update the zh_CN.po file from the kicad.pot file using PO Editor

- Open PO Editor
- Load the zh_CN.po file
- Select Translation -> Update from pot file , and update the zh_CN.po file from the kicad.pot file
- Save the zh_CN.po file

3. Apply the HQ changes

```bash
pip install polib
python ./translation/apply_distribution_json.py ./translation/distribution.json   ./translation/pofiles/zh_CN.po
```

## Debug HQ Features env vars:

- KICAD_HQ_DEBUG_WEBVIEW : Enable the webview debug tool
- KICAD_HQ_DEBUG_COPILOT : Enable the copilot debug console

