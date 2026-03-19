# HQ Distribution

## Update Translations

1. Update the zh_CN.po file

```bash
./translation/update-po-files.sh 
```

2. Apply the HQ changes

```bash
pip install polib
python ./translation/apply_distribution_json.py ./translation/distribution.json   ./translation/pofiles/zh_CN.po
```
