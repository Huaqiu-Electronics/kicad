# Migrating the hq features from 9.x to 10.x

## Generate the commits

```bash
git checkout release/9.0
git log --author="Ethan Chien" --author="Eric" --author="Liu Weijian" --pretty=format:"%H|%ad|%s" --date=iso --reverse > 9.0-hq-distribution.log
```
## Start cherry-pick

python ./cherry_pick_resume.py