import subprocess
import sys
import os

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


def load_failed_shas():
    if not os.path.exists(FAILED_FILE):
        return set()
    failed = set()
    with open(FAILED_FILE, "r", encoding="utf-8") as f:
        for line in f:
            line = line.strip()
            if not line:
                continue
            sha = line.split("|", 1)[0].strip()
            failed.add(sha)
    return failed


def record_failure(commit):
    with open(FAILED_FILE, "a", encoding="utf-8") as f:
        f.write(f"{commit['sha']} | {commit['date']} | {commit['message']}\n")


def run_cmd(cmd):
    return subprocess.run(cmd, shell=True)


def main():
    commits = load_commits()
    failed_shas = load_failed_shas()
    start_index = load_state()

    print(f"Total commits: {len(commits)}")
    print(f"Resuming from index: {start_index}")
    print(f"Already failed commits: {len(failed_shas)}")

    for i in range(start_index, len(commits)):
        commit = commits[i]
        sha = commit["sha"]

        # Skip previously failed commits
        if sha in failed_shas:
            print(f"[{i+1}/{len(commits)}] Skipping previously failed {sha}")
            save_state(i + 1)
            continue

        print(f"\n[{i+1}/{len(commits)}] Cherry-picking {sha}")
        print(f"Message: {commit['message']}")

        result = run_cmd(f"git cherry-pick -x {sha}")

        if result.returncode != 0:
            print(f"❌ Conflict detected on {sha}, aborting script.")
            record_failure(commit)
            save_state(i + 1)
            sys.exit(1)  # fail-fast

        save_state(i + 1)

    print("\n✅ All commits processed.")


if __name__ == "__main__":
    main()