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

%% à compléter


Un premier programme _user_
---------------------------

## Q12 Plan d'adressage _user_

%% à completer

## Q16 activation _user land_

%% à completer

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

