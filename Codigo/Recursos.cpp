#include "Recursos.h"

// Definiciones de las variables globales
Texture pisoTexture;
Texture aguaTexture;

Model Faro;
Model pilarFaro;

Model CentroPokemon_M;
Texture centroPokemonTexture;

Model Arco_M;
Texture stoneTexture;

Model PortonIzquierdo_M;
Model PortonDerecho_M;
Texture blackMetalTexture;

Model Letrero_M;
Texture letreroTexture;

Model Torii_M;
Texture toriiTexture;

Model Puerto_M;
Texture puertoTexture;

Model Trajinera_M;
Texture trajineraTexture;

Model Ichiraku_M;
Texture ichirakuTexture;

Model PuestoTacos_M;
Texture puestoTacosTexture;

Texture humoTexture;

Model PuestoElotes_M;
Texture puestoElotesTexture;

Model PuestoPozole_M;
Texture puestoPozoleTexture;

Model PuestoPlantas_M;
Texture puestoPlantasTexture;

Model PuestoRecuerdos_M;
Texture puestoRecuerdosTexture;

Model PuestoLuchas_M;
Texture puestoLuchasTexture;

Model Bangboo_M;
Texture bangbooTexture;

Model Stool_M;
Texture stoolTexture;

Texture farolaTexture;
Model Farola_M;

Texture pokeFanTexture;
Model Pokefan_M;

Texture pokeshopTexture;
Model Pokeshop_M;

Texture piramideTexture;
Model Piramide_M;

Texture calendarioMayaTexture;

Texture hawluchaTexture;
Model Hawlucha_M;

Model HawluchaCuerpo_M;
Model HawluchaBrazoIzq_M;
Model HawluchaBrazoDer_M;
Model HawluchaPiernaIzq_M;
Model HawluchaPiernaDer_M;

Texture ringTexture;
Model Ring_M;

Texture caminoTexture;
Model Camino_M;

Texture sillaTexture;
Model Silla_M;
Model Silla2_M;

Texture pidgeyTexture;
Model Pidgey_M;

Texture rayquazaTexture;
Model Rayquaza_M;

Texture squirtleTexture;
Model Squirtle_M;

Texture mudkipTexture;
Model Mudkip_M;

Texture charmanderTexture;
Model Charmander_M;

Texture pidoveTexture;
Model Pidove_M;

Texture krabbyTexture;
Model Krabby_M;

Texture machampTexture;
Model Machamp_M;

Texture globoTexture;
Model Globo_M;

Texture pastoTexture;
Model Pasto_M;

Texture boteBasuraTexture;
Model BoteBasura_M;

Texture vallaUnoTexture;
Model VallaUno_M;

Texture vallaDosTexture;
Model VallaDos_M;

void cargarRecursos() {
    
	pisoTexture = Texture("Textures/grass_texture.tga");
	pisoTexture.LoadTextureA();

	aguaTexture = Texture("Textures/agua.tga");
	aguaTexture.LoadTextureA();

	Faro = Model();
	Faro.LoadModel("Models/BaseF.obj");

	pilarFaro = Model();
	pilarFaro.LoadModel("Models/pilarF.obj");

	CentroPokemon_M = Model();
	CentroPokemon_M.LoadModel("Models/centroPokemon.obj");

	centroPokemonTexture = Texture("Textures/centroPokemon.tga");
	centroPokemonTexture.LoadTextureA();

	Ichiraku_M = Model();
	Ichiraku_M.LoadModel("Models/ichiraku.dae");

	ichirakuTexture = Texture("Textures/ichiraku.png");
	ichirakuTexture.LoadTextureA();

	Arco_M = Model();
	Arco_M.LoadModel("Models/Arco.dae");

	stoneTexture = Texture("Textures/stone.tga");
	stoneTexture.LoadTextureA();

	PortonIzquierdo_M = Model();
	PortonIzquierdo_M.LoadModel("Models/Gate-Left.dae");
	PortonDerecho_M = Model();
	PortonDerecho_M.LoadModel("Models/Gate-Left.dae");

	blackMetalTexture = Texture("Textures/black_metal.tga");
	blackMetalTexture.LoadTextureA();

	Letrero_M = Model();
	Letrero_M.LoadModel("Models/Letrero.dae");
	letreroTexture = Texture("Textures/letrero.tga");
	letreroTexture.LoadTextureA();

	Puerto_M = Model();
	Puerto_M.LoadModel("Models/Puerto.dae");
	puertoTexture = Texture("Textures/harbor_texture.png");
	puertoTexture.LoadTextureA();

	Trajinera_M = Model();
	Trajinera_M.LoadModel("Models/trajinera.dae");
	trajineraTexture = Texture("Textures/trajinera.tga");
	trajineraTexture.LoadTextureA();

	Torii_M = Model();
	Torii_M.LoadModel("Models/torii.dae");
	toriiTexture = Texture("Textures/torii.png");
	toriiTexture.LoadTextureA();

	humoTexture = Texture("Textures/humo.png");
	humoTexture.LoadTextureA();

	PuestoTacos_M = Model();
	PuestoTacos_M.LoadModel("Models/puesto_tacos.dae");
	puestoTacosTexture = Texture("Textures/taco_texture.png");
	puestoTacosTexture.LoadTextureA();

	PuestoElotes_M = Model();
	PuestoElotes_M.LoadModel("Models/carroElote.dae");
	puestoElotesTexture = Texture("Textures/tostiesquites.png");
	puestoElotesTexture.LoadTextureA();

	PuestoPozole_M = Model();
	PuestoPozole_M.LoadModel("Models/puestopozole.dae");
	puestoPozoleTexture = Texture("Textures/puestopozole.png");
	puestoPozoleTexture.LoadTextureA();

	PuestoPlantas_M = Model();
	PuestoPlantas_M.LoadModel("Models/puesto_plantas.dae");
	puestoPlantasTexture = Texture("Textures/puesto_plantas.png");
	puestoPlantasTexture.LoadTextureA();

	PuestoRecuerdos_M = Model();
	PuestoRecuerdos_M.LoadModel("Models/puestorecuerdos.dae");
	puestoRecuerdosTexture = Texture("Textures/puestorecuerdos.png");
	puestoRecuerdosTexture.LoadTextureA();

	PuestoLuchas_M = Model();
	PuestoLuchas_M.LoadModel("Models/puestoLuchas.dae");
	puestoLuchasTexture = Texture("Textures/puestoLucha.png");
	puestoLuchasTexture.LoadTextureA();

	Bangboo_M = Model();
	Bangboo_M.LoadModel("Models/bangboo.dae");
	bangbooTexture = Texture("Textures/file1.001.png");
	bangbooTexture.LoadTextureA();
	
	Stool_M = Model();
	Stool_M.LoadModel("Models/stool.dae");
	stoolTexture = Texture("Textures/file2.001.png");
	stoolTexture.LoadTextureA();

	farolaTexture = Texture("Textures/texture_lamp.tga");
	farolaTexture.LoadTextureA();

	Farola_M = Model();
	Farola_M.LoadModel("Models/Farola.fbx");
	
	pokeFanTexture = Texture("Textures/pokefan.png");
	pokeFanTexture.LoadTextureA();
	Pokefan_M = Model();
	Pokefan_M.LoadModel("Models/pokefan.dae");

	pokeshopTexture = Texture("Textures/pokeshop.png");
	pokeshopTexture.LoadTextureA();
	Pokeshop_M = Model();
	Pokeshop_M.LoadModel("Models/pokeshop.dae");

	piramideTexture = Texture("Textures/piramidetexture.png");
	piramideTexture.LoadTextureA();
	Piramide_M = Model();
	Piramide_M.LoadModel("Models/piramide.dae");

	calendarioMayaTexture = Texture("Textures/calendario_maya.tga");
	calendarioMayaTexture.LoadTextureA();

	hawluchaTexture = Texture("Textures/hawlucha.png");
	hawluchaTexture.LoadTextureA();

	Hawlucha_M = Model();
	HawluchaCuerpo_M.LoadModel("Models/hawlucha_cuerpo.dae");
	HawluchaBrazoIzq_M.LoadModel("Models/hawlucha_brazo_i.dae");
	HawluchaBrazoDer_M.LoadModel("Models/hawlucha_brazo_d.dae");
	HawluchaPiernaIzq_M.LoadModel("Models/hawlucha_pierna_i.dae");
	HawluchaPiernaDer_M.LoadModel("Models/hawlucha_pierna_d.dae");

	ringTexture = Texture("Textures/ring.tga");
	ringTexture.LoadTextureA();
	Ring_M = Model();
	Ring_M.LoadModel("Models/ring.dae");

	caminoTexture = Texture("Textures/mosaico.png");
	caminoTexture.LoadTextureA();
	Camino_M = Model();
	Camino_M.LoadModel("Models/camino.dae");

	sillaTexture = Texture("Textures/silla.png");
	sillaTexture.LoadTextureA();
	Silla_M = Model();
	Silla_M.LoadModel("Models/silla.dae");
	Silla2_M = Model();
	Silla2_M.LoadModel("Models/silla_plegada.dae");

	pidgeyTexture = Texture("Textures/pidgey.png");
	pidgeyTexture.LoadTextureA();
	Pidgey_M = Model();
	Pidgey_M.LoadModel("Models/pidgey.dae");

	rayquazaTexture = Texture("Textures/rayquaza.png");
	rayquazaTexture.LoadTextureA();
	Rayquaza_M = Model();
	Rayquaza_M.LoadModel("Models/rayquaza.dae");

	squirtleTexture = Texture("Textures/squirtle.png");
	squirtleTexture.LoadTextureA();
	Squirtle_M = Model();
	Squirtle_M.LoadModel("Models/squirtle.dae");

	mudkipTexture = Texture("Textures/mudkip.png");
	mudkipTexture.LoadTextureA();
	Mudkip_M = Model();
	Mudkip_M.LoadModel("Models/mudkip.dae");

	charmanderTexture = Texture("Textures/charmander.png");
	charmanderTexture.LoadTextureA();
	Charmander_M = Model();
	Charmander_M.LoadModel("Models/charmander.dae");

	pidoveTexture = Texture("Textures/pidove.png");
	pidoveTexture.LoadTextureA();
	Pidove_M = Model();
	Pidove_M.LoadModel("Models/pidove.dae");

	krabbyTexture = Texture("Textures/krabby.png");
	krabbyTexture.LoadTextureA();
	Krabby_M = Model();
	Krabby_M.LoadModel("Models/krabby.dae");

	machampTexture = Texture("Textures/machamp.png");
	machampTexture.LoadTextureA();
	Machamp_M = Model();
	Machamp_M.LoadModel("Models/machamp.dae");

	globoTexture = Texture("Textures/globo.png");
	globoTexture.LoadTextureA();
	Globo_M = Model();
	Globo_M.LoadModel("Models/globo.dae");

	pastoTexture = Texture("Textures/kusamura.png");
	pastoTexture.LoadTextureA();
	Pasto_M = Model();
	Pasto_M.LoadModel("Models/pasto.dae");

	boteBasuraTexture = Texture("Textures/trash_can.png");
	boteBasuraTexture.LoadTextureA();
	BoteBasura_M = Model();
	BoteBasura_M.LoadModel("Models/trash_can.dae");

	vallaUnoTexture = Texture("Textures/valla1.png");
	vallaUnoTexture.LoadTextureA();
	VallaUno_M = Model();
	VallaUno_M.LoadModel("Models/valla1.dae");

	vallaDosTexture = Texture("Textures/valla1.png");
	vallaDosTexture.LoadTextureA();
	VallaDos_M = Model();
	VallaDos_M.LoadModel("Models/valla2.dae");
}