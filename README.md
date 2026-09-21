# NintendoWare4CTR

* This project is dedicated to the NintendoWare4Ctr engine which is a commonly used engine in 3DS games. Please note not EVERY function is here. Only known ones.

## --Libraries--

| `anim` | **anim**ation Library | Handles skeletal & material animations |

| `eft`  | **ef**ec**t** Emitter | Post 2014 effect rendering |

| `font` | **font** | Font handling |

| `gfx`  | **g**raphics **fx** | Model & effects rendering |

| `h3d`  | **h**itrus **3d** | Post-2014 advanced graphics library |

| `io`   | **i**/**o** | Input/Output handling |

| `lyt`  | **l**a**y**ou**t** | Window & 2D UI system |

| `math` | **math** | Math Utility Library |

| `os`   | **o**perating **s**ystem | Low-level OS utilities |

| `snd`  | **s**ou**nd** | Sound engine for developers |

| `ut`   | **ut**ility | General-purpose utilities |


---

## Prerequisites to use Project
- [CTRSDK](https://github.com/LoigiFan72/CTRSDK) — SDK dependency used by `all 3DS Projects.`

## References
- [NW4F SouND](https://github.com/nsmbu/Snd) - Huge credit here. Major starting pointing to the undocumented SouND library headers wise. I forked these for a starting point to these.
- [NW4R SouND](https://github.com/doldecomp/ogws/tree/master/src/nw4r/snd) - NintendoWare4R's Sound Library reference. I used these as a general reference on what it could look like for Ctr
- [NW4R LayYouT](https://github.com/doldecomp/ogws/tree/master/src/nw4r/lyt) - NintendoWare4R's Lyt private functions.
- [NW4R UTility](https://github.com/doldecomp/ogws/tree/master/include/nw4r/ut) - NintendoWare4R's UTility macro inspiration

## Debug Binaries
- There are several binaries which contain ethier just symbols, or DWARF for NW4C. These binaries are...
| Game Name (Version)| Map File "Where" | Description
| `CTRDash (MK7)(DLP Child)` | YES "romfs:/CTRDash.xmap" | Just symbols.
| `AliceP (Zelda TriForce)(DLP Debug Child)` | YES "" | Just symbols. Map file doesn't match build, so it will need to be adjusted.