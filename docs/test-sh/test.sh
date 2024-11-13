#!/bin/sh

# Script CGI pour afficher un contenu HTML simple

# En-têtes HTTP obligatoires
echo "Content-type: text/html charset=utf-8\r\n\r\n"
#echo ""

# Début du contenu HTML
echo "<!DOCTYPE html>"
echo "<html>"
echo "<head><title>Script CGI en Shell</title></head>"
echo "<body>"
echo "<h1>Bonjour, ceci est un script CGI en Shell !</h1>"

# Affichage des variables d'environnement (utile pour le débogage)
echo "<h2>Variables d'environnement :</h2>"
echo "<pre>"
env | sort
echo "</pre>"

echo "<p>La requête a été traitée avec succès.</p>"
echo "</body>"
echo "</html>"
