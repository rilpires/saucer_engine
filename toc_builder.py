import os

allowed_extensions = ["png","node","lua","wav","ttf","glsl","config"]

all_resources = []

for R,D,F in os.walk("res"):
    for f in F:
        res_path = os.path.join(R,f)
        res_extension = res_path.split(".")[-1].lower()
        if res_extension in allowed_extensions:
            all_resources.append(res_path)

with open("toc","w") as f:
    for r in all_resources:
        f.write(r)
        f.write('\n')
    f.close()
