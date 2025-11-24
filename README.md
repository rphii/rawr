# rawr

**r**phii's **a**wesomely **w**onky **r**enderer

## 📦 Install
```shell
meson setup build
meson install -C build
```

## ⚙️ Usage

### stream any data into an image (encode)

```
rawr encode file.txt -o out.png
```

```
rawr encode file1.txt file2.txt -o out2.png
```


### stream data out from image (decode)

```
rawr decode out.png -o file.txt
```

```
rawr decode out2.png -o concatenated.txt
```

## SOON™

- stream through pipes and redirect
- make output optionally verbose (not as it is now, always showing)

