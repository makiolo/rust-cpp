# About

## Sort

``` py title="bubble_sort.py"
def bubble_sort(items):
    for i in range(len(items)):
        for j in range(len(items) - 1 - i):
            if items[j] > items[j + 1]:
                items[j], items[j + 1] = items[j + 1], items[j]
```

## Prueba formato

Text can be {--deleted--} and replacement text {++added++}. This can also be
combined into {~~one~>a single~~} operation. {==Highlighting==} is also
possible {>>and comments can be added inline<<}.

{==

Formatting can also be applied to blocks by putting the opening and closing
tags on separate lines and adding new lines between the tags and the content.

==}

## Table

| Method      |     | Description                          |
| ----------- |-----| ------------------------------------ |
|             |     |                                      |
| `GET`       |     | :material-check:     Fetch resource  |
| `PUT`       |     | :material-check-all: Update resource |
| `DELETE`    |     | :material-close:     Delete resource |

## Example Python

```py
import os
os.add_dll_directory(r"C:\Program Files\GTK3-Runtime Win64")
from weasyprint import HTML
HTML('http://weasyprint.org/').write_pdf('/tmp/weasyprint-website.pdf')
```

## Maths

[//]: <> ( Editor Latex: https://cortexjs.io/compute-engine/demo/ )
$$
\mu=\frac{1}{n}\sum_{i\mathop{=}0}^jx_i
$$

## Experiments

Python normal

```python
print("Hello Markdown!")
```

Python with exec

```python exec="on"
print("Hello Markdown!")
```

