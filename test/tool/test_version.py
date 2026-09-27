import argparse
import os
import re
import subprocess

import pytest

import dsqss
from dsqss import version

VERSION_STRING = re.compile(r"^v\S+ \(([0-9a-f]{8}(-dirty)?|unknown)\)$")


def write_cmakelists(root, line):
    with open(os.path.join(str(root), "CMakeLists.txt"), "w") as f:
        f.write("cmake_minimum_required(VERSION 3.1)\n")
        f.write("project(DSQSS NONE)\n")
        f.write(line + "\n")


# ---- version number in CMakeLists.txt ----

@pytest.mark.parametrize(
    "line, expected",
    [
        ("set(DSQSS_VERSION 2.1.0)", "2.1.0"),
        ("set(DSQSS_VERSION v2.1.0)", "2.1.0"),
        ('set(DSQSS_VERSION "2.2-dev")', "2.2-dev"),
        ("set( DSQSS_VERSION  v2.2-dev )", "2.2-dev"),
        ("  set(DSQSS_VERSION 2.2rc1)", "2.2rc1"),
    ],
)
def test_version_from_cmakelists(tmp_path, line, expected):
    write_cmakelists(tmp_path, line)
    assert version.version_from_cmakelists(str(tmp_path)) == expected


def test_version_from_cmakelists_without_version(tmp_path):
    write_cmakelists(tmp_path, "# set(DSQSS_VERSION 2.1.0)")
    assert version.version_from_cmakelists(str(tmp_path)) == "unknown"


def test_version_from_cmakelists_without_file(tmp_path):
    assert version.version_from_cmakelists(str(tmp_path)) == "unknown"


# ---- commit hash of a tarball ----

HASH = "0123456789abcdef0123456789abcdef01234567"


def test_git_hash_from_git_hash_file(tmp_path):
    (tmp_path / ".git_hash").write_text(HASH + "\n")
    assert version.git_hash_from_source(str(tmp_path)) == "01234567"


def test_git_hash_from_archival_file(tmp_path):
    (tmp_path / ".git_archival.txt").write_text(HASH + "\n")
    assert version.git_hash_from_source(str(tmp_path)) == "01234567"


def test_git_hash_from_archival_file_not_filled_in(tmp_path):
    (tmp_path / ".git_archival.txt").write_text("$Format:%H$\n")
    assert version.git_hash_from_source(str(tmp_path)) == "unknown"


def test_git_hash_file_precedes_archival_file(tmp_path):
    (tmp_path / ".git_hash").write_text(HASH + "\n")
    (tmp_path / ".git_archival.txt").write_text("$Format:%H$\n")
    assert version.git_hash_from_source(str(tmp_path)) == "01234567"


def test_git_hash_from_git_hash_file_of_dirty_tree(tmp_path):
    (tmp_path / ".git_hash").write_text(HASH + "-dirty\n")
    assert version.git_hash_from_source(str(tmp_path)) == "01234567-dirty"


def test_git_hash_without_information(tmp_path):
    assert version.git_hash_from_source(str(tmp_path)) == "unknown"


@pytest.mark.parametrize(
    "content",
    ["", "0123456\n", "not a hash\n", "0123456g\n", "0123456-dirty\n", "-dirty\n"],
)
def test_git_hash_from_broken_file(tmp_path, content):
    (tmp_path / ".git_hash").write_text(content)
    assert version.git_hash_from_source(str(tmp_path)) == "unknown"


# ---- commit hash of a git repository ----

def git(root, *args):
    subprocess.check_call(
        ["git", "-c", "user.name=test", "-c", "user.email=test@example.com"]
        + list(args),
        cwd=str(root),
        stdout=subprocess.DEVNULL,
        stderr=subprocess.DEVNULL,
    )


def make_repository(root):
    try:
        git(root, "init")
        (root / "tracked.txt").write_text("committed\n")
        git(root, "add", "tracked.txt")
        git(root, "commit", "-m", "test")
    except (OSError, subprocess.CalledProcessError):
        pytest.skip("git is not available")
    head = subprocess.check_output(
        ["git", "rev-parse", "HEAD"], cwd=str(root), universal_newlines=True
    )
    return head.strip()[:8]


def test_git_hash_from_repository(tmp_path):
    make_repository(tmp_path)
    # the files for a tarball are not used in a repository
    (tmp_path / ".git_hash").write_text(HASH + "\n")

    head = subprocess.check_output(
        ["git", "rev-parse", "HEAD"], cwd=str(tmp_path), universal_newlines=True
    ).strip()
    assert version.git_hash_from_source(str(tmp_path)) == head[:8]
    assert head[:8] != HASH[:8]


def test_git_hash_with_modified_file(tmp_path):
    head = make_repository(tmp_path)
    (tmp_path / "tracked.txt").write_text("modified\n")
    assert version.git_hash_from_source(str(tmp_path)) == head + "-dirty"


def test_git_hash_with_staged_file(tmp_path):
    head = make_repository(tmp_path)
    (tmp_path / "new.txt").write_text("new\n")
    git(tmp_path, "add", "new.txt")
    assert version.git_hash_from_source(str(tmp_path)) == head + "-dirty"


def test_git_hash_with_removed_file(tmp_path):
    head = make_repository(tmp_path)
    (tmp_path / "tracked.txt").unlink()
    assert version.git_hash_from_source(str(tmp_path)) == head + "-dirty"


def test_git_hash_with_untracked_file(tmp_path):
    head = make_repository(tmp_path)
    (tmp_path / "untracked.txt").write_text("not under the version control\n")
    assert version.git_hash_from_source(str(tmp_path)) == head


def test_git_hash_after_changes_are_reverted(tmp_path):
    head = make_repository(tmp_path)
    (tmp_path / "tracked.txt").write_text("modified\n")
    assert version.git_hash_from_source(str(tmp_path)) == head + "-dirty"
    (tmp_path / "tracked.txt").write_text("committed\n")
    assert version.git_hash_from_source(str(tmp_path)) == head


def test_git_hash_from_repository_without_commit(tmp_path):
    try:
        git(tmp_path, "init")
    except (OSError, subprocess.CalledProcessError):
        pytest.skip("git is not available")
    assert version.git_hash_from_source(str(tmp_path)) == "unknown"


# ---- version of the package ----

def test_version_is_known():
    assert dsqss.__version__ != "unknown"
    assert not dsqss.__version__.startswith("v")


def test_version_string_format():
    assert VERSION_STRING.match(dsqss.version_string())
    assert dsqss.version_string() == "v{} ({})".format(
        dsqss.__version__, dsqss.git_hash()
    )


def test_version_action(capsys):
    parser = argparse.ArgumentParser()
    parser.add_argument("--version", action=dsqss.VersionAction)
    with pytest.raises(SystemExit) as e:
        parser.parse_args(["--version"])
    assert e.value.code == 0
    assert capsys.readouterr().out == dsqss.version_string() + "\n"


def test_version_action_is_optional():
    parser = argparse.ArgumentParser()
    parser.add_argument("--version", action=dsqss.VersionAction)
    args = parser.parse_args([])
    assert not hasattr(args, "version")


@pytest.mark.parametrize(
    "module",
    [
        "dla_alg",
        "dla_pre",
        "parameter",
        "pmwa_pre",
        "std_lattice",
        "std_model",
        "wavevector",
    ],
)
def test_tools_show_version(module, capsys, monkeypatch):
    import importlib

    main = importlib.import_module("dsqss." + module).main
    monkeypatch.setattr("sys.argv", [module, "--version"])
    with pytest.raises(SystemExit) as e:
        main()
    assert e.value.code == 0
    assert capsys.readouterr().out == dsqss.version_string() + "\n"
