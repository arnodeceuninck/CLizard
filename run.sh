# dit bestand zal gerund worden en moet voor de nodige input en output zorgen
# je mag hier aanpassingen doen, maar dezelfde functionaliteit moet ondersteund worden
# als er een bestand niet aangemaakt wordt, krijg je daar ook geen punten voor
#
# in dit bestand wordt er van uit gegaan dat er een executable met de naam indiv staat
# dat volgende parameters ondersteunt:
# <algoritme> <inputfile> <outputfile(s)>:
# - algoritme is een uit cnf, cyk, ll1, lr, multitm, tm, tmsub (voor subroutines), pda2cfg
# - inputfile is het inputbestand (let op bij tm zijn dat er 3)
# - outputfile is het outputbestand (indien de output geen pda is, geef je telkens de dot output van de input)
# het is de bedoeling dat hieronder enkel de lijnen komen van de algoritmes die je ondersteunt

# CNF: de output bestaat uit alle tussenstappen: toon elke grammatica in een goed leesbare vorm, dus niet in json 
# en toon telkens de nullables, unit pairs, generating en reachables
./indiv cnf input-cnf1.json output-cnf1.txt
./indiv cnf input-cnf2.json output-cnf2.txt

# CYK: de output bestaat uit de resulterende tabel, niet in dot formaat, bv in html en de conclusie van CYK
# de input bestaat uit de cfg en de gegeven inputstring
./indiv cyk input-cyk1.json baaba output-cyk1-baaba.txt
./indiv cyk input-cyk1.json abba output-cyk1-abba.txt
./indiv cyk input-cyk2.json aaabb output-cyk2-aaabb.txt
./indiv cyk input-cyk2.json aaabbb output-cyk2-aaabbb.txt

# LL1: de output bestaat uit de resulterende parse table, niet in dot, bv in html, tonen van First en Follow sets
./indiv ll input-ll1.json output-ll1.txt
./indiv ll input-ll2.json output-ll2.txt

# LR1
./indiv lr input-lr1.json output-lr1.txt
./indiv lr input-lr2.json output-lr2.txt

# TM
./indiv tm input-tm1.json output-tm1.dot
./indiv tm input-tm2.json output-tm2.dot

# multitm
./indiv multitm input-multitm1.json output-multitm1.dot
./indiv multitm input-multitm2.json output-multitm2.dot

# subroutines
./indiv sub input-tm1.json input-tm2.json output-tm1-tm2.dot

# PDA2CFG: je geeft de resulterende cfg in een leesbare vorm
./indiv pda2cfg input-pda2cfg1.json output-pda2cfg1.txt
./indiv pda2cfg input-pda2cfg2.json output-pda2cfg2.txt
