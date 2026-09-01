# NintendoWare4CTR

> A Nintendo library suite used in most 3DS games.

## --Libraries--

| Module | Name | Description |
|--------|------|--------------|
| `anim` | **ANIM**ation Library | Handles skeletal & material animations |
| `font` | **FONT** Library | Font rendering (`BCFNT`, `BFFNT` (ver 2 of BCFNT)) |
| `gfx`  | **G**raphics **FX** Library | Model & effects rendering (`BCMDL` + more) |
| `io`   | **I/O** Library | Input/Output handling |
| `lyt`  | **L**a**Y**ou**T** Library | Window & UI layout system |
| `math` | **MATH** Utility Library | Math Library (uses [nn::math](https://github.com/LoigiFan72/CTRSDK/tree/main/include/nn/math)) |
| `os`   | **O**perating**S**ystem Utility Library | Low-level OS utilities |
| `snd`  | **S**ou**ND** System Library | Sound engine for developers (`BSCAR`) |
| `ut`   | **U**tility Library | General-purpose utilities |

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