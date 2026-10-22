#!/usr/bin/env python3
"""Create a ZIP (or ZIP-based .pack) without a bundled archiver."""
import argparse
from fnmatch import fnmatch
from pathlib import Path
import zipfile


def create_zip(destination, source, *, stored=False, include_root=False, exclude_patterns=()):
    destination, source = Path(destination).resolve(), Path(source).resolve()
    if not source.is_dir():
        raise ValueError(f"Not a directory: {source}")
    destination.parent.mkdir(parents=True, exist_ok=True)
    temporary = destination.with_suffix(destination.suffix + ".tmp")
    base = source.parent if include_root else source
    try:
        compression = zipfile.ZIP_STORED if stored else zipfile.ZIP_DEFLATED
        with zipfile.ZipFile(temporary, "w", compression=compression) as archive:
            for path in sorted(source.rglob("*")):
                relative = path.relative_to(source)
                if (path.is_file() and path not in (destination, temporary)
                        and ".svn" not in relative.parts
                        and not any(fnmatch(path.name.lower(), p.lower()) for p in exclude_patterns)):
                    archive.write(path, path.relative_to(base).as_posix())
        temporary.replace(destination)
    finally:
        temporary.unlink(missing_ok=True)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("archive", type=Path)
    parser.add_argument("source", type=Path)
    parser.add_argument("--stored", action="store_true")
    parser.add_argument("--include-root", action="store_true")
    parser.add_argument("--exclude-pattern", action="append", default=[])
    args = parser.parse_args()
    create_zip(args.archive, args.source, stored=args.stored, include_root=args.include_root,
               exclude_patterns=args.exclude_pattern)


if __name__ == "__main__":
    main()
