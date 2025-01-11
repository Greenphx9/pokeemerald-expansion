# show-givemon
This is a fork of the pokeemerald-expansion. By setting `show` to be 1 in your givemon call, the species name is buffered to string buffer 1, the nature is buffered to string buffer 2, and the shiniess of the mon is set to VAR_TEMP_B. It also modifies showmonpic to allow showing shinies, which when used with VAR_TEMP_B, shows if the given mon is shiny. This feature branche is based off Unbound.

## Usage

Here is an example script:
```
EventScript_Text_ReceivedWithNature::
	.string "{PLAYER} received a {STR_VAR_1}!\n"
	.string "It's nature is {STR_VAR_2}.$"

EventScript_GiveMonWithNature::
	givemon SPECIES_BULBASAUR, 20, show=TRUE
	showmonpic SPECIES_BULBASAUR, 10, 3, VAR_TEMP_B
	msgbox Debug_Text1, MSGBOX_DEFAULT
	hidemonpic
	release
	end
```

## Credits
- Skeli789: this feature is inspired by Unbound.