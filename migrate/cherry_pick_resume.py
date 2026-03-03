#!/usr/bin/env python3
import subprocess
import sys
import os
from pathlib import Path

LOG_FILE = "9.0-hq-distribution.log"
STATE_FILE = "cherry_pick_state.txt"
FAILED_FILE = "cherry_pick_failed.txt"


def load_commits():
    commits = []
    with open(LOG_FILE, "r", encoding="utf-8") as f:
        for line in f:
            line = line.strip()
            if not line:
                continue
            sha, date, message = line.split("|", 2)
            commits.append({
                "sha": sha,
                "date": date,
                "message": message
            })
    return commits


def load_state():
    if not os.path.exists(STATE_FILE):
        return 0
    with open(STATE_FILE, "r") as f:
        return int(f.read().strip())


def save_state(index):
    with open(STATE_FILE, "w") as f:
        f.write(str(index))


def record_failure(commit):
    with open(FAILED_FILE, "a") as f:
        f.write(f"{commit['sha']} | {commit['date']} | {commit['message']}\n")


def run_cmd(cmd):
    return subprocess.run(cmd, shell=True)


def main():
    commits = load_commits()
    start_index = load_state()

    print(f"Total commits: {len(commits)}")
    print(f"Resuming from index: {start_index}")

    for i in range(start_index, len(commits)):
        commit = commits[i]
        sha = commit["sha"]

        print(f"\n[{i+1}/{len(commits)}] Cherry-picking {sha}")
        print(f"Message: {commit['message']}")

        result = run_cmd(f"git cherry-pick {sha}")

        if result.returncode != 0:
            print("❌ Conflict detected or error occurred.")
            record_failure(commit)
            print("Resolve conflicts manually, then run:")
            print("    git cherry-pick --continue")
            print("After that, re-run this script to continue.")
            sys.exit(1)

        save_state(i + 1)

    print("\n✅ All commits processed.")


if __name__ == "__main__":
    main()