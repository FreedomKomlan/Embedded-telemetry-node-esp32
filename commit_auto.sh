#!/bin/bash

# Message de commit par défaut avec la date
COMMIT_MESSAGE="Commit on $(date '+%Y-%m-%d %H:%M'): Add TasksManager class and refactor main.cpp, msg_queue.hpp will add another task to read msg_queue and send to console"

# Aller dans le dossier du projet (à adapter)
cd "C:\Users\kfree\Documents\Data_Elcool\Projets\Software_Dev\SY-36_ESPX" || exit

# Ajouter tous les fichiers modifiés
git add .

# Faire le commit
git commit -m "$COMMIT_MESSAGE"

# Pousser sur la branche principale (à adapter si tu utilises une autre branche)
git push origin main