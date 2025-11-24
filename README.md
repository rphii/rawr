# rawr

**r**phii's **a**wesomely **w**onky **r**enderer

Encode and decode arbitrary data in .png files.

## 🔗 Dependencies

- `rphii/rlc` (core extensions)
- `rphii/rlso` (string object)
- `rphii/rlarg` (argument parser)
- `nothings/stb` (image handling)
- math library (`-lm`)

## 📦 Install

Dependencies are handled by meson, except for `nothings/stb`.

```shell
meson setup build
meson install -C build
```

## ⚙️ Usage

### stream any data into an image (encode)

Single file:

```
rawr encode file.txt -o out.png
```

Concat two files:

```
rawr encode file1.txt file2.txt -o out2.png
```


### stream data out from image (decode)

Single file:

```
rawr decode out.png -o file.txt
```

Originally three files, now written into one file:

```
rawr decode out.png out2.png -o concatenated.txt
```

## SOON™

- stream through pipes and redirect
- make output optionally verbose (not as it is now, always showing)
- respect endianness

