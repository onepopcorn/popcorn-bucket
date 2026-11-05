# Keyboard Control
A very basic example to show how to control user input via keyboard.

## What you learn
The goal of this example is to show how to detect and interface with the user input via keyboard. For that, we will install a handler to interrupt IRQ1 (0x09) and see how to handle both, keys being pressed and keys just pressed to prevent bouncing events for actions we just one to happens once per keystroke.

The example also shows how there are more keys that available keycodes and some keys are prefixed with a special byte and how to handle those.

<figure align="center">
    <img src="demo.png" />
    <figcaption>Keyboard input</figcapture>
</figure>