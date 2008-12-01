#ifndef LEVELMANAGER_HPP
#define LEVELMANAGER_HPP

#include <queue>
#include <string>

#include "tinyxml/tinyxml.h"
#include "Entity.hpp"

/*
 * Parse le fichier des niveaux et instancie les unités
 * les unités seront ensuite envoyées à Game
 */
class LevelManager
{
public:
	enum Error
	{
		SUCCESS ,
		FILE,   // Erreur d'e-s
		UNDEF,  // Niveau non défini
		PARSE,  // Erreur de parsing
		END	 // Fin de chaînage  
	};
	
	/*
	 * Récupérer l'instance unique
	 * @return référence sur le gestionnaire de niveaux
	 */
	static LevelManager& GetInstance();
	
	/*
	 * Obtenir la prochaine unité du niveau
	 * @param[in] timer: temps écoulé
	 * @return entity s'il y en a au moins une qui doit aparaître avant timer
	 * sinon, NULL (il n'y pas d'entity avant timer)
	 */
	Entity* GiveNext(float timer);
	
	/*
	 * Obtenir le nombre d'unités restantes dans la file d'attente du niveau
	 */
	inline int RemainingEntities() const
	{
		return waiting_line_.size();
	};
	
	/*
	 * Définit le level courant, affecte à Description le contenu de l'attribut description, 
	 * s'îl est non vide.
	 */
	Error Set(int level);
	
	/*
	 * Obtenir la description d'un niveau
	 * @param[in] level: indice du niveau
	 * @return chaîne de description
	 */
	const char* GetDescription(int level) const;
	
	/*
	 * Retourne le nuémro du dernier niveau
	 */
	inline int GetLast()
	{
		return levels_.size();
	};
	
private:
	struct EntitySlot
	{
		Entity* self;
		float spawntime;
	};
	
	LevelManager();
	~LevelManager();
	
	Error ParseFile(const char* file);
	
	Error ParseLevel(TiXmlElement* elem);
	
	void Purge();
	
	std::queue<EntitySlot> waiting_line_;
	std::vector<TiXmlElement*> levels_;
	TiXmlDocument doc_;
};

#endif /* guard LEVELMANAGER_HPP */

