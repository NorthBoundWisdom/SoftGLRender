#!/bin/bash

# set the clang-format path
CLANG_FORMAT="C:/Program Files/LLVM/bin/clang-format.exe"

# set the target directory
TARGET_DIR="C:/Users/EthanKang/Documents/SoftGLRender/src/Render"
# switch to the target directory
cd "$TARGET_DIR" || exit

# counter
count=0
total=$(find . \( -name "*.cpp" -o -name "*.h" \) | wc -l)

# find all .cpp and .h files and process them one by one
find . \( -name "*.cpp" -o -name "*.h" \) -print0 | while IFS= read -r -d '' file; do
    ((count++))
    echo "[$count/$total] Formatting: $file"
    "$CLANG_FORMAT" -i "$file"

    # ensure the file ends with a newline
    sed -i -e '$a\' "$file"
done

echo "Formatting complete. $total files processed in $TARGET_DIR"
