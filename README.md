# Far5a-Trollware

## Far5a-Trollware is a benign malware; software made to troll users and scare them. It is benign, it doesn't do harm.
### It is an applied project during my study for Malware Analysis/Development and Windows System Internals. If you wanna secure yourself from adversaries, you have to think like one.

## Demo


## Lore
There is an old joke circulating around my friends, telling "العيد فرخة" (Feast is chicken) instead of "العيد فرحة" (Feast glares with joy).
So a friend suggested making a malware that turns everything as roast chicken and they all voted that I should make it. Hence making this trollware and calling it Far5a-trollware which translates to Chicken-trollware.

## Useful snippets
You can find some useful snippets to aid your studies in Malware Development and Windows System internals
- Changing desktop background
- Displaying a video on the desktop background
- Synchronizing audio with those changes
- Changing icons for files
- Changing icons for folders
- Registering Services on autostart
- MAD (Mutually Assured Destruction) Processes
- Process Code Injector
- Reverting everything back and cleaner script
- Self-deletion after execution

## Understanding its mechanism
Far5a heavily relies on Windows API functions, combining lots of capabilities supported by the WinAPI in a creative way to achieve horror.
It also practices changing Keys in the registry while saving the default values to be able to revert everything back.
Victim's files are intact, everything is revertible, this malware comes with its anti-malware.
Code files are heavily documented, reading comments is like reading a story; Each file says what it does, each function is explained, so you can read what went on my mind during development.

## Limitations
While this trollware is not statically detectable by Windows Defender, it is easily detectable when it is run because of the heavy reliance on Windows API functions which are hooked by Windows Defender.
To evade runtime detection, I must use manual syscalls instead of the Windows API functions, which is very cumbersome.

## Reason of development
I am a malware analyst, I'm in a constant phase of studying malwares. As part of my studies for malware development and Windows system internals, Far5a was my playground to test my extent of manual malware development.

## What it does
1) Displays an introduction video on the desktop to build thrill and tension
2) Registers hidden services that play scary sounds regularly
3) Persists those sounds to make it hard for users to manually delete them
4) Turns all your computer icons to roast chickens
5) Displays a rotating roast chicken on your desktop

## How to run
1) Compile your own version of Far5a via the set of commands inside ./src/Compile.txt
2) Run the top-level executable which is Far5a.exe, you must run it as administrator to see its full extent.

## How to stop it in-case things get unexpected
1) Press f8 to revert desktop and icons to their previous states.
2) Open a CMD as administrator
3) Copy what's inside ./src/Cleaner.txt into the CMD
4) Press enter to clean your registry and kill any scary service/process 

## Disclaimer and Warnings
1) Do not use it as weaponry, I've already uploaded it to virustotal and virusshare to be detected by most AVs and EDRs.
2) Do not Try to run it without knowing how to stop it (it is easily stoppable, I've written how to stop it)
3) It doesn't do any harm, all files are left intact.
4) It is an educational project as part of my studies in malware development and windows system internals
5) I am not responsible for anyone who uses parts of my code to do something malicious, all evil is already easily accessible via the internet.
