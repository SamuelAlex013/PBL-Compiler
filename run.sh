"./core/output/lexer.exe"

BASE_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

# Define source and destination paths
SRC="$BASE_DIR/tokens.txt"
DEST_DIR="$BASE_DIR/build/Desktop_Qt_6_9_0_MinGW_64_bit-Release/release/core/output"
DEST="$DEST_DIR/tokens.txt"

# Create destination directory if it doesn't exist
mkdir -p "$DEST_DIR"

# Move the file
mv -f "$SRC" "$DEST"

echo "Moved tokens.txt to release folder successfully!"


# Run visualizer
"./build/Desktop_Qt_6_9_0_MinGW_64_bit-Release/release/PBLCompilerVisualizer.exe"