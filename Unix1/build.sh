#!/bin/sh

# Запоминаем текущую директорию
MAIN_DIR="$PWD"

cleanup() {
    exit_code=$?
    if [ -n "$tmpdir" ] && [ -d "$tmpdir" ]; then
        rm -rf "$tmpdir"
    fi
    exit $exit_code
}

trap cleanup EXIT INT TERM HUP QUIT

if [ $# -ne 1 ]; then
    echo "Usage: $0 <source_file>" >&2
    exit 1
fi

src_file="$1"
if [ ! -f "$src_file" ]; then
    echo "Error: Source file '$src_file' not found" >&2
    exit 2
fi

output_name=$(grep '^[[:space:]]*//[[:space:]]*Output:' "$src_file" | head -1 | sed 's/.*Output:[[:space:]]*//' | tr -d '\r\n')

if [ -z "$output_name" ]; then
    echo "Error: No output file specified in comments" >&2
    echo "Add a comment like: // Output: filename" >&2
    exit 3
fi

if ! tmpdir=$(mktemp -d); then
    echo "Error: Failed to create temporary directory" >&2
    exit 4
fi

cp "$src_file" "$tmpdir/"
cd "$tmpdir"

filename=$(basename "$src_file")
if echo "$filename" | grep -q '\.c$'; then
    if ! gcc "$filename" -o "$output_name" 2>errors.log; then
        cat errors.log >&2
        echo "Error: Compilation failed" >&2
        exit 5
    fi
elif echo "$filename" | grep -q '\.cpp$\|\.cxx$\|\.cc$'; then
    if ! g++ "$filename" -o "$output_name" 2>errors.log; then
        cat errors.log >&2
        echo "Error: Compilation failed" >&2
        exit 5
    fi
else
    echo "Error: Unsupported file type" >&2
    exit 6
fi

# Возвращаемся в исходную директорию и копируем файл
cd "$MAIN_DIR"
cp "$tmpdir/$output_name" ./

echo "SUCCESS: File '$output_name' created in current directory"
exit 0
