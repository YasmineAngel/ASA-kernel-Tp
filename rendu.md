Ce fichier est le rendu de <PRENOM> <NOM> & <PRENOM> <NOM>
==========================================================

Définir des structures de données MMU
-------------------------------------




Définir un plan d'adressage
---------------------------

## Q5 Adresse de compilation

from observing the link.ld file : the compiled code is tored at the physical address : 0x00100000
The other sections should be stored consecutively after the .text section with a 4096 byte alignment .

## Q6 Mémoire virtuelle et noyau

- Since the virtual address corresponds to physiscal address the idx1 and idx2 of the my-kernel code can be caclculated :
    - Kernel address = 0x00100000 = 0000 0000 0001 0000 0000 0000 0000 0000
    - idx1 (top 10 bits of the virtual address) : 0000 0000 00
    - idx2 (next 10 bits): 01 0000 0000
- And the idx1 and idx2 of the video memory address used by putc:
    - Video memory address : 0xB8000 = 0000 0000 0000 1011 1000 0000 0000 0000
    - idx1 :  0000 0000 00
    - idx2 : 00 1011 1000



## Q7 Espace mémoire

The number of memory pages that will configured is 1024 . Which corresponds to 1024*4096 bytes = 4194304 bytes =~ 4.19 Mb of memory .

## Q8 Plan d'adressage

- Configuration of page directory table : 
    we are only required to configure the idx1 (the first 10 bits) which will point to corresponding physical address of the page_table
- Configuration of page entry table : 
    Here we should configure all entries to the corresponding physiscal page where the first 20 bits correspond to the  physiscal page address (ex: page_table[0] points to physical address 0x00000000) , set the mode to kernel , set it as a valid entry and activate read/write bit .




Un premier programme _user_
---------------------------

## Q12 Plan d'adressage _user_

- Address range : 0x00400000 to 0x007FFFFF
- Smallest available Address : 0x00400000 (index 1 * 1024 entries * 4096 bytes = 4 194 304 bytes)
- largest accessible address : 0x007FFFFF (the end of the second 4 MB range (2 * 4,194,304) - 1 = 8 388 607 bytes)


## Q16 activation _user land_

I observe the message : error accessing address 40000 over and over again .
why ? : call_user jumped to the 4MB mark (0x400000), but the CPU hit a Page Fault because it doesn't have permission to read the memory there.
After changing user_page_table[j].mode to 1 in setup_mmu() the error is gone .

Correction
----------

## Q18 chercher la faute

%% à completer

Interposition
-------------

## Q22 Interposition d’appel

%% à completer

protection de pile
------------------

## Q29 Analyse

%% à completer

Extension dynamique de pile
---------------------------

## Q34 Pile d'appel virtuelle

%% à completer

