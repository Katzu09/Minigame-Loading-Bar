# SA:MP Minigame System

A simple minigame system for SA:MP that displays a progress bar and target line that players need to hit.

## Features

- Auto-moving progress bar
- Target line that needs to be hit
- Anti-spam system
- Hit counter system
- Callback support for minigame results
- Automatic cleanup on player disconnect

## Usage

1. Include the minigame.inc file in your gamemode:
```pawn
#include <minigame>
```

2. Start the minigame for a player:
```pawn
StartPlayerMinigame(playerid, hittowin = 5, Float:speedbar = 0.3, successmsg[] = "Success!", failmsg[] = "Failed!", winmsg[] = "You won!", callbackname[] = "");
```

3. Player must press ALT button to try hitting the target line
4. If they successfully hit the target hittowin times, the minigame ends

## Parameters

- `playerid` - ID of the player who will play the minigame
- `hittowin` - Number of hits needed to win (default: 5)
- `speedbar` - Speed of bar movement (default: 0.3)
- `successmsg` - Message shown on successful hit
- `failmsg` - Message shown on failed hit
- `winmsg` - Message shown on winning
- `callbackname` - Name of callback to be called when minigame ends (optional)

## Example Usage

```pawn
public OnPlayerCommandText(playerid, cmdtext[])
{
    if(!strcmp(cmdtext, "/minigame", true))
    {
        StartPlayerMinigame(playerid, 5, 0.3, "Nice hit!", "Miss!", "You win!", "OnMinigameComplete");
        return 1;
    }
    return 0;
}

public OnMinigameComplete(playerid)
{
    // Give reward to player
    GivePlayerMoney(playerid, 1000);
    return 1;
}
```

## Notes

- Make sure y_hooks is included
- Use StopPlayerMinigame() to forcefully stop the minigame
- Minigame will automatically stop when player disconnects
- There is a cooldown anti-spam when pressing ALT

## Credits

Created by: Katzu09
Version: 1.0

## License

MIT License