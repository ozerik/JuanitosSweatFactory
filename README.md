# JuanitosSweatFactory
Eurorack CV sequencer with radically flexible hardware
<h2>Juanito's Sweat Factory</h2>
<p>Here's a brand new Eurorack project (April 2025 new) and I need to organize the to-do list, that's what this is right now.</p>
<p><strong>First, the concept:</strong></p>
<ul>
<li>four CV inputs at the top</li>
<li>four potentiometers under the jacks</li>
<li>the outside potentiometers have a microswitch under them for clickyclicky</li>
<li>middle of the module -- a circle of 8 potentiometers that will control CVs</li>
<li>in the center of the circle there's a keyboard keyswitch for "shift" duties?</li>
<li>on the bottom there's 8 output jacks.Th</li>
<li>every pot is a clear-shafted JuanitoPot, with an addressable APA102 LED under it</li>
<li>every jack has an LED under it, just a single color, doesn't flash or anything</li>
<li>each output jack has an LED near it that shines with whatever intensity the jack is outputting</li>
</ul>
<p>The hardware is built, and works more or less as expected although of the first five prototypes, only TWO will allow themselves to be programmed. I'll optimize prototype 2 with better trace routing, but I really don't know exactly what's going on</p>
<p><strong>Now the hardware:</strong></p>
<p>There's a microcontroller sitting in the middle of this project. I chose the AVR128DB64 because it's cheap, has LOTS of pins, it's easy to program for, and it's plenty fast. The top left input jack and the pot near it each send a signal to the microcontroller. The other three jacks and pots up there get analog-mixed before the voltage goes to a pin. The eight pots in a circle each get their own pin. The top-left output is from the microcontroller's built-in DAC. The other three outputs in that row are PWM-analog outputs. The bottom four outputs are conceptually for ON or OFF gates, but they can be PWM'ed as well, and simple 1-pole filtering can be added with a capacitor per channel. The addressable LEDs are in a single string.</p>
<p>So it's a microcontroller sitting in the middle of a spider-web of inputs and outputs. My own concept for this is to be a CV sequencer, variable clock math, pre-set patterns, variable lengths and directionss, at least one envelope generator, portamento between CV steps, gates out, and more things I will think of later. </p>
<p>There's a CH340 programmer for the chip onboard, meaning all you'll need to develop for this platform is a USB cable and software. The AVR128Dx chips are well=supported by Arduino, at least, and for sure other more grownup IDEs.</p>
<p><strong>Now the code</strong></p>
<p>I have just built my first prototypes, so the next step is CODE. Ugh. </p>
<ol>
  <li>get all the analog and the three switch values into the microcontroller as efficiently as possible, using variables and constants that make sens</li>
  <li>get the addressable LEDs driven and labeled in a way that mirrors the names of the potentiometers</li>
  <li>set up self-clocking and clock divisions, as well as getting that jack set up as a clock input. Not sure if an interrupt routine is worth the trouble, maybe watching for the clock in the loop will be fast enough. But heavens, that's a lot of pins getting analogRead();</li>
</ol>
<p>okay more to come</p>
