#!/usr/bin/env python3

# Importation des modules nécessaires
import cgi
import cgitb
import datetime

# Activation du débogage CGI
cgitb.enable()

# Définition de l'en-tête HTTP pour l'affichage en HTML
print("HTTP/1.1 200 OK")
print("Content-Type: text/html")
print()  # Ligne vide nécessaire entre l'en-tête et le contenu

# Récupérer l'heure actuelle
heure_actuelle = datetime.datetime.now().strftime("%H:%M:%S")

# Code HTML affichant l'heure
print(f"""
<html>
<head>
    <title>Heure Actuelle</title>
</head>
<body>
    <h1>Heure Actuelle</h1>
    <p>L'heure actuelle est : <strong>{heure_actuelle}</strong></p>
</body>
</html>
""")
