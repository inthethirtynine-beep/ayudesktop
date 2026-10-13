import sys

exe_path = sys.argv[1]
with open(exe_path, "rb") as f:
    data = f.read()

count_utf8 = data.count(b"AyuGram")
data = data.replace(b"AyuGram", b"FlyGram")

count_lower = data.count(b"ayugram")
data = data.replace(b"ayugram", b"flygram")

count_upper = data.count(b"AYUGRAM")
data = data.replace(b"AYUGRAM", b"FLYGRAM")

u16_src = "AyuGram".encode("utf-16le")
u16_dst = "FlyGram".encode("utf-16le")
count_u16 = data.count(u16_src)
data = data.replace(u16_src, u16_dst)

u16_src_l = "ayugram".encode("utf-16le")
u16_dst_l = "flygram".encode("utf-16le")
count_u16_l = data.count(u16_src_l)
data = data.replace(u16_src_l, u16_dst_l)

with open(exe_path, "wb") as f:
    f.write(data)

print(f"Branding patched successfully! UTF8={count_utf8}, lower={count_lower}, upper={count_upper}, UTF16={count_u16}, UTF16_lower={count_u16_l}")
