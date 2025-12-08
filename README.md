# QMK Userspace

Original documentation may be found at: https://docs.qmk.fm/newbs_external_userspace

# Splitkb Elora rev1

This is a repository for a personal keymap configuration of the [Splitkb's Elora rev1](https://splitkb.com/products/elora).

Below are the listed features of this keyboard firmware

## Thumb cluster definition 

For the purpose of this documentation, here is the definition for the thumb cluster:

```
            ,-------.    ,-------.
,-----------|TL3|TL2|    |TR2|TR3|-----------.
|TL7|TL6|TL5|TL4|TL1|    |TR1|TR4|TR5|TR6|TR7|
"-------------------'    "-------------------'

```

## QMK Features

Keyboard makes use of the following QMK features:

* Caps word

* Combo keys
  * See symbol pad definition in the next section about layers

* Tap Dance
  * See custom lefty definition on the next section about layers
  * TL7 is Home on single-tap, but Page Up on double-tap
  * TR7 is End on single-tap, but Page Down on double-tap

* Layer switching
  * Switching between default layouts: [engram](https://github.com/binarybottle/engram) and QWERTY
  * One shot layer

* Repeat key
  * See custom key codes about space bar
  * Normal repeat key defined at TR3

* Custom key codes
  * Differentiates the left and right space. 
    * Hitting them individually sends out a normal space
    * Hitting them simultaneously sends out the repeat key

* Buzzer sounds
  * Clicky keys
  * Music map

## Layers

* Momentary layers are accessed by the inner-most thumb cluster keys. 
  * Momentary layer access is defined specifically on keys TR1, TR2, TL1, and TL2
  * Layers are prioritized per-half. This firmware follows the ideology: only the half that the thumb activates should have the keys defined. 
    * This is subject to change with future experimentation. It is also subject to being loosely defined, in that lesser keys could be thrown on the other half.

* Number pad layer: accessed by holding TR1.
  * flattened numberpad (numberpad layer: Home row has 0-4, top row has 5-9)

* Function pad layer: accessed by holding TR2.
  * Flattened function keys (same idea as the numberpad). 
  * Has the keys for system media and music map controls and clicky keys control. 
  * Has other functions keys such as the locking keys (scroll, caps, numberpad) and insert.
    * These have not been considered for optimization as they are seldomly used and there is no motivation to optimize their placement further.

* Symbol layer: attempts to prioritize general strongly-types languages. 
  * Makes use of combo keys. Looking at it from the perspective of QWERTY: 
    * `c` sends `{`, `d` sends `<`, `e` sends `(`, and `3` sends `[`
    * `cv` sends `}`, `df` sends `>`, `er` sends `)`, and `34` sends `]`

* Changing RGB lights to indicate function or layer activation
  * Lefty activates a blue hue with red keypress highlights
  * Number pad, symbol pad, and function pad keys have a yellow color indicator they are pressed
  * QWERTY and engram, as well as the gaming layer, use different LED animations

* Custom lefty layer: has two variants.
  * Used for one-handed activations and typing. Note that QMK has its own method for this called [swap hands](https://docs.qmk.fm/features/swap_hands), 
  but that is not implemented here as this has a layout that is a little more custom.
  * Has two variants
    * A [One shot](https://docs.qmk.fm/one_shot_keys) variant noramlly accessed by pressing TL2
    * A double-tap variant making use of QMK's tap dance feature; accessed by holding TR1 and pressing TL2
      * Single-tap sends out the left-half key
      * Double-tap sends out the respective right-half key

* Gaming layer: still experimental (not a lot of gaming is done on MnK as much as controller).
  * Plays a portions of the start of the Helldivers 2 theme when entering and exiting the gaming layer.
  * Places the number keys around the hand, , 1...9 and 0 clockwise around the left hand.
    * A layer change positions the function keys in the same way
  * Repositions the WASD to go over the keys, in the perspective of QWERTY, `sdfc`
    * A layer change rewrites these as the respective arrow keys
