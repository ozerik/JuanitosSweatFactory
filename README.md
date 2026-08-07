# JuanitosSweatFactory
Eurorack CV sequencer with radically flexible hardware
<h2>Juanito's Sweat Factory</h2>
<p>Here's a brand new Eurorack project (April 2025 new) and I need to organize the to-do list, that's what this is right now.</p>
<p><strong>First, the concept:</strong></p>
<p>This represents the software I've written so far, as of August 6th. Tweaks and bugfixes will happen.</p>
<ul>
<li>four CV inputs at the top</li>
<li>four potentiometers under the jacks</li>
<li>the outside potentiometers have a microswitch under them for clickyclicky</li>
<li>middle of the module -- a circle of 8 potentiometers that will control CVs</li>
<li>in the center of the circle there's a keyboard keyswitch for "shift" duties?</li>
<li>on the bottom there's 8 output jacks.</li>
<ol>
<li>The top left output is connected to a real DAC. </li>
<li>The next one over is an envelope output. </li>
<li>The 3rd one over is a gate outut, which goes HIGH (+10V) every time the output CV is greater than basically zero.</li>
<li>The right-most jack is a clock output, which triggers once per step in the sequence. <i>There's spots on the PCB to add a capacitor to allow the non-CV outputs to be filtered to use PWM-style for more analog voltages out, if you want to develop software to do that.</i></li>
<li>The jack right under the main CV out has the same CV output as the jack above it. When the user <i>forks</i> the pattern, (hold SHIFT key and turn CLOCK knob up) the output of this jack plays back the sequence as recorded. The main sequence can be altered, but the recorded sequence will continue looping. This behavior carries across for the next two outputs...</li>
<li>This guy is the secondary envelope output.</li>
<li>Here's the secondary gate output.</li>
<li>This last output jack is the "BOC" or Beginning of Cycle trigger. When the sequence restarts from "zero" this output sends a trigger.</li>
</ol>
<li>Every pot is a clear-shafted JuanitoPot, with an addressable APA102 LED under it. They're all 100K linear pots, but 10K pots might be "better" for impedance reasons, but nobody has built 10K JuanitoPots yet.</li>
<li>Every jack has an LED under it, just a single color, doesn't flash or anything</li>
<li>Each output jack has an LED near it that shines with whatever intensity the jack is outputting</li>
</ul>

<p><strong>Now the hardware:</strong></p>
<p>There's a microcontroller sitting in the middle of this project. I chose the AVR128DB64 because it's cheap, has LOTS of pins, it's easy to program for, and it's plenty fast. The top left input jack and the pot near it each send a signal to the microcontroller. The other three jacks and pots up there get analog-mixed before the voltage goes to a pin. The eight pots in a circle each get their own pin. The top-left output is from the microcontroller's built-in DAC. The other three outputs in that row are PWM-analog outputs. The bottom four outputs are conceptually for ON or OFF gates, but they can be PWM'ed as well, and simple 1-pole filtering can be added with a capacitor per channel. The addressable LEDs are in a single string.</p>
<p>So it's a microcontroller sitting in the middle of a spider-web of inputs and outputs. My own concept for this is to be a CV sequencer, variable clock math, pre-set patterns, variable lengths and directionss, at least one envelope generator, portamento between CV steps, gates out, and more things I will think of later. </p>
<p>There's a CH340 programmer for the chip onboard, meaning all you'll need to develop for this platform is a USB cable and software. The AVR128Dx chips are well-supported by Arduino, at least, and for sure other more grownup IDEs.</p>
<p></b>Now the code</b></p>
<p>The first prototypes had problems. The second prototypes were closer. The third prototype will tweak the programming wiring to make flashing more reliable (serial communications have turned out to be a nightmare, WHAT AM I DOING WRONG?) and have tweaked power rails plus a proper voltage reference.</p>

<h1>USER MANUAL!</h1>
<H2>Turn it on!</h2>
<strong>Clocks</strong>
<p>This module can be externally or internally clocked. The top left CLOCK knob is the way to interface with the clock.</p>
<p><b>External clock</b></p>
<p>Your incoming clock signal can be divided or multiplied, or passed to the module one-to-one. <b>CONFIRM THIS PART LATER</b> Pressing and turning the clock pot all the way counter-clockwise allows you to send one pattern-advance-signal to the sequencer per 24 triggers. This follows the MIDI standard of 24PPQN (pulses per quarter note). With the pot in the middle range, the incoming triggers will pass to the sequencer one-to-one, a trigger moves the sequencer to the next step. With the pot turned clockwise, the sequencer will advance twice per incoming trigger. <b>IS THIS TRUE WHY WOULD I HAVE CODED IT LIKE THIS</b></p>
<p><b>Internal clock</b></p>
<p>Turning the knob while <i>not</i> pressing it down adjusts the clock division parameter. There's a fast clock running in the background, which allows all the recording of portamento and envelope values at about 24PPQN; fast enough for glides or envelopes.</p>
<p>Divisions include 32nd notes, 16th notes, triplets, quarter notes, whole notes, multiples of whole notes, plenty of options that will allow variations <i>without</i> changing the base clock tempo.</p>
<p>Pressing the knob while turning it alters the base clock speed, so you can fine-tune how fast your sequence advances.</p>
<p>Holding the knob down while tapping the SHIFT key is tap-tempo. It's pretty good, but no matter how exact you are with tapping, don't expect the tapped temp to match the tempo of something else for longer than a minute or so.</p>
<strong>Patterns</strong>
<p><b>Mode</b></p>
<p>The MODE knob selects which type of pattern will play. From counter-clockwise to clockwise:</p>
<ol>
<li><b>Linear</b> Pattern goes up or down, configured by METAMODOE knob</li>
<li><b>Cylon</b> Pattern goes clockwise until it reaches the end, configured by METAMODE knob, then it goes counter-clockwise until it gets to "step 1" which is to say the "noon" knob</li>
<li><b>Pre-set Patterns</b> Here's three banks of "songs" with various lengths of pattern, from <i>Stayin' Alive</i> to <i>The Chain</i>. There's no way to tell which "song" is being played, and without <i>VERY CAREFULLY</i> setting each knob to the correct CV, the "songs" aren't going to sound like the song, but the patterns are valid, and can sound amazing if you set the CVs right. The patters are up to 64 steps, with room in the current code for 128-step patterns. Probably not room in RAM for 256-step patterns. You can make your own patterns, but it's a weird hassle that I can't really explain. Look at the code, which includes a "song_pattern_workshop.ino" file that can help. Pre-recorded patterns include envelope triggers.<li>
<li><b>Circle Knob Select Mode</b> When turned all the way clockwise, the knob turns white, and METAMODE knob chooses which potentiometer in the circle is "active". This allows tuning your knobs without a pattern messing you up.</li>
</ol>
<p>The METAMODE knob is next, but we've already gone over its functions.</p>
<p><i>Oh, by the way</i> the jacks above the MODE and METAMODE pots allow CV-control over these two parameters. Dunno why anybody would want that, but we're a creative community with crazy ideas.</p>
<strong>Envelopes</strong>
<p>The ENVELOPE knob. This part will almost certainly change. Or at least, a different way to adjust the ADSR will be added.</p>
<p>First off, the jack associated with this knob feeds gate signals to the envelope generator. How do you set the parameters of the envelope generator? That comes later.</p>
<p>It's later now. So: there's a four-parameter envelope, Attack, Decay, Sustain, Release. ADSR. The module boots up and instantly applies <i>whatever-value-the-envelope-knob-is</i> to each stage of the envelope. Clicking the ENVELOPE knob cycles between stages of the envelope.</p>
<p>Holding the ENVELOPE knob kinda "plays" the envelope through the knob's LED. It shines in white, going through the stages, with a half-second "sustain" stage.</p>
<ol>
<li>Attack. Green. CCW = fast, CW = slow.</li>
<li>Decay. Purple. CCW = fact, CW = slow.</li>
<li>Sustain. Yellow-orange. CCW = zero CV output, CW = high CV output.</li>
<li>Release. Ice-blue. CCW = fast, CW = very slow indeed.</li>
</ol>
<strong>A Word About Knobs and How To Pick Them Up</strong>
<p>The CLOCK and ENVELOPE knobs, and other knobs at times, will modify a parameter while you don't want to modify the underlying default parameter. So when you flip back to default, and the knob has changed, the knob will be either blue or red, brighter or dimmer. Red? Gotta turn the knob down until you <i>catch</i> the previous knob position. Blue? Turn the knob up! The brightness of the color will decrease the closer you get.</p>
<p><i>Look for updates soon that will turn the top 4 knobs into ADSR adjusters while in config mode.</i></p>
<strong>Circle Pots</strong>
<p>Super basic: each knob turns the CV associated with it up or down. Other stuff gets done when the SHIFT key is pressed, or when CFG mode is enabled (by double-clicking the SHIFT key).</p>
<strong>Shift Key Plus Knobs And Stuff</strong>
<ul>
<li><b>Hold-Plus-Circle-Pot</b> Turn the CIRCLEPOT up, enable slew or glide or portamento on. There's two levels, short and long. Short is 300mS, long is 1.5 seconds. The knob will turn pink, dimmer or brighter, to show that glide has been enabled, short or long. Turn the CIRCLEPOT down, and it'll turn off glide for that pot.</li>
<li><b>Hold-Plus-Clock-Pot</b> actually this one is reverse. Hold CLOCK pot, tap SHIFT, do tap-tempo. We already talked about this.</li>
<li><b>Hold-Plus-Envelope-Pot</b> Turn the ENVELOPE knob up and you've <i>forked the pattern!</i> This decouples the performed pattern from the pattern that has been being recorded the whole time. I think the pattern length may even be decoupled, but I'm not sure about that. <i>NOPE</i> the pattern length obeys the performed pattern. This is possible to change, there's room in the pattern variable. Turn the ENVELOPE down, and you've gone back to recording the performed pattern into the secondary pattern.</li>
<li><b>Hold-While-In-Fork-Mode</b> Well now, this turns on the "recorded" gate output.</li>
<li><b>While-In-Fork-Mode-Hold-Shift-Press-ENVELOPE-Knob</b> Okay, this turns off the "recorded" gate output.</li>
<li><b>(related) Press-Envelope-Knob-While-In-Fork-Mode</b> This records new CVs into the "recorded" CV output. While in Fork Mode, I think the jack near the envelope knob stops functioning as a gate input for the envelope generator, and becomes a CV input for the gesture recorder. Not sure. I'll figure that out and code it to behave that way.</li>
</ul>
<strong>Configure Mode</strong>
<p><b>Get into Configure Mode by double-clicking the shift key</b></p>
<p>While in Cofigure mode, most of the circle pots go dark, with a blink of light following whatever pattern is currently playing. There's three knobs still lit, the one at 9 o'clock, at 6 o'clock, and 3 o'clock. Or west, south, and east, since this is an 8-point circle.</p>
<p>WEST knob controls the range of the output CV, from 0 to 10V, down to only 0 to 1V. All CV alterations happen at once, there's no "pickup" strategy here.</p>
<p>SOUTH knob controls the brightness of all the knob LEDs. UNFORTUNATELY, the jack LEDs and the LEDs associated with each output are NOT adjustable, and my eyeballs tear up in sympathy with yours if you happen to get one of the prototype modules with egregious levels of brightness.</p>
<p>EAST knob controls quantization. All the way counter-clockwise is passthrough, no quantization. Then the scales go:</p>
<ol>
<li>Chromatic</li>
<li>Major</li>
<li>Minor</li>
<li>Major Pentatonic</li>
<li>Minor Pentatonic</li>
<li>Harmonic Minor</li>
<li>Whole Tone</li>
<li>blues</li>
</ol>
<p>There's other scales out there, and other tunings to play with, but let's leave that up to creative developers out there to fiddle with.</p>
<p></p>
<p></p>
<p>Okay that's about it. Look for ENVELOPE controls across the top four knobs. I'm confident there's bugs, especially in the external clock system? That part was a hassle to code.</p>