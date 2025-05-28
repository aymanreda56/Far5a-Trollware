import sys

def remove_null_bytes(input_path, output_path):
    with open(input_path, 'rb') as f_in:
        data = f_in.read()

    cleaned = data.replace(b'\x00', b'')

    with open(output_path, 'wb') as f_out:
        f_out.write(cleaned)

    #print(f"Removed {data.count(b'\x00')} null bytes and saved to {output_path}")

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python remove_null_bytes.py <input_file> <output_file>")
        sys.exit(1)

    remove_null_bytes(sys.argv[1], sys.argv[2])
