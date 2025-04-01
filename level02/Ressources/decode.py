hex_values = [
    '756e505234376848',  # First chunk
    '45414a3561733951',  # Second chunk
    '377a7143574e6758',  # Third chunk
    '354a35686e475873',  # Fourth chunk
    '48336750664b394d'   # Fifth chunk
]

password = ''
for hex_val in hex_values:
    bytes_val = bytes.fromhex(hex_val)
    password += bytes_val[::-1].decode('ascii')

print(password)