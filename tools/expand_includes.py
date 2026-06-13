#!/usr/bin/env python3

import argparse
import os
import re
import sys
from pathlib import Path


INCLUDE_RE = re.compile(r'^(\s*)#\s*include\s*([<"])([^>"]+)[>"]')
GUARD_RE = re.compile(
    r'^\s*#\s*ifndef\s+[A-Z0-9_]*_HPP\s*$'
    r'|^\s*#\s*define\s+[A-Z0-9_]*_HPP(?:\s+\S+)?\s*$'
    r'|^\s*#\s*endif\s*(?://\s*)?[A-Z0-9_]*_HPP\s*$'
)
DEFAULT_OUTPUT = "combined.cpp"


class Expander:
    def __init__(self, include_dirs):
        self.include_dirs = [Path(p).resolve() for p in include_dirs]
        self.included = set()

    def resolve(self, name, delimiter, current_dir):
        candidates = []
        if delimiter == '"':
            candidates.append(current_dir / name)
        if name.startswith("atcoder/"):
            candidates.extend(directory / name for directory in self.include_dirs)
            candidates.extend(directory / "ac-library" / name for directory in self.include_dirs)
        elif delimiter == '"':
            candidates.extend(directory / name for directory in self.include_dirs)
        else:
            return None

        for candidate in candidates:
            if candidate.is_file():
                return candidate.resolve()
            hpp = candidate.with_name(candidate.name + ".hpp")
            if hpp.is_file():
                return hpp.resolve()
        return None

    def should_skip_header_line(self, line):
        stripped = line.strip()
        if stripped == "#pragma once":
            return True
        if GUARD_RE.match(line):
            return True
        return False

    def expand_file(self, path):
        path = path.resolve()
        if path in self.included:
            return []
        self.included.add(path)

        result = []
        current_dir = path.parent
        try:
            lines = path.read_text().splitlines()
        except UnicodeDecodeError:
            lines = path.read_text(encoding="utf-8").splitlines()

        for line in lines:
            if self.should_skip_header_line(line):
                continue

            match = INCLUDE_RE.match(line)
            if match:
                delimiter = match.group(2)
                name = match.group(3)
                resolved = self.resolve(name, delimiter, current_dir)
                if resolved is not None:
                    result.append(f"// begin include: {name}")
                    result.extend(self.expand_file(resolved))
                    result.append(f"// end include: {name}")
                    continue

            result.append(line)
        return result

    def expand(self, source):
        self.included = set()
        source = Path(source).resolve()
        return "\n".join(self.expand_file(source)) + "\n"


def default_include_dirs(source):
    dirs = [Path.cwd(), source.parent, Path.cwd() / "libraries", Path.cwd() / "ac-library"]
    env = os.environ.get("CPLUS_INCLUDE_PATH", "")
    dirs.extend(Path(p) for p in env.split(os.pathsep) if p)

    unique = []
    seen = set()
    for directory in dirs:
        resolved = directory.resolve()
        if resolved not in seen:
            seen.add(resolved)
            unique.append(resolved)
    return unique


def main():
    parser = argparse.ArgumentParser(
        description="Expand local and ACL includes into a single C++ source file."
    )
    parser.add_argument("source", help="input C++ source file")
    parser.add_argument(
        "-o",
        "--output",
        default=DEFAULT_OUTPUT,
        help=f"output file (default: {DEFAULT_OUTPUT})",
    )
    parser.add_argument(
        "-I",
        "--include",
        action="append",
        default=[],
        help="additional include directory",
    )
    parser.add_argument(
        "--stdout",
        action="store_true",
        help="print expanded source instead of writing a file",
    )
    args = parser.parse_args()

    source = Path(args.source)
    include_dirs = default_include_dirs(source.resolve()) + [Path(p).resolve() for p in args.include]
    expander = Expander(include_dirs)
    expanded = expander.expand(source)

    if args.stdout:
        sys.stdout.write(expanded)
    else:
        output = Path(args.output)
        output.parent.mkdir(parents=True, exist_ok=True)
        output.write_text(expanded)


if __name__ == "__main__":
    main()
