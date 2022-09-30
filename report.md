---
author: Kristoffer Plagborg Bak Sørensen
mail: 201908140@post.au.dk
date: 30-09-2022
---

# Wireless Sensor Networks - Assignment 1

## Methodology

$$
	x = \cos() + \cos() + wgn(0,1)
$$

```python
def gen_test_signal(N: int) -> np.ndarray:
    noise = np.random.normal(0, 0.1, N)
    signal = (
        5 * np.cos(np.linspace(0, 10, N) * 2)
        + np.cos(np.linspace(0, 10, N) * 8 - np.pi / 2)
        + noise
    )
    return signal
```


## Results


### time

| experiment ID | $dt$ in arch platform ticks | $dt$ in seconds |
|---|---|---|
| | | |

## Conclusion
