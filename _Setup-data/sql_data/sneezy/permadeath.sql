-- MariaDB dump 10.19  Distrib 10.5.9-MariaDB, for debian-linux-gnu (x86_64)
--
-- Host: sneezy-db    Database: sneezy
-- ------------------------------------------------------
-- Server version	10.5.9-MariaDB-1:10.5.9+maria~focal

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8mb4 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Table structure for table `permadeath`
--

DROP TABLE IF EXISTS `permadeath`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `permadeath` (
  `name` varchar(80) NOT NULL DEFAULT '',
  `level` int(11) DEFAULT NULL,
  `died` int(11) DEFAULT NULL,
  `killer` varchar(80) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `permadeath`
--

LOCK TABLES `permadeath` WRITE;
/*!40000 ALTER TABLE `permadeath` DISABLE KEYS */;
INSERT INTO `permadeath` VALUES ('Telaqui',5,1,'a robber'),
('Fujisaowa',7,0,'no one'),
('Anda',11,1,'fire'),
('Yoritomo',2,1,'a strong man'),
('Tonniji',3,0,'no one'),
('Nastradamus',15,1,'a mud worker'),
('Wheyisk',4,0,'no one'),
('Greywulf',3,1,'a gypsy'),
('Mickle',13,0,'no one'),
('Reign',2,1,'a good-sized rat'),
('Ashame',6,0,'no one'),
('Wesp',6,1,'buggy shaman code'),
('Grunter',5,0,'no one'),
('Hermione',10,0,'no one'),
('Anthoes',3,1,'a <B>cityguard<1>'),
('Fink',10,0,'no one'),
('Jacen',12,1,'a mud guard'),
('Farina',16,1,'the woodelven wife'),
('Vaedos',2,0,'no one'),
('Julius',11,0,'no one'),
('Frood',2,1,'a <B>city watchman<1>'),
('Guffaw',6,1,'a robber'),
('Trillea',12,0,'no one'),
('Gichin',8,1,'Gichin'),
('Keros',2,0,'no one'),
('Khaleefa',16,0,'no one'),
('Tendo',2,0,'no one'),
('Durok',32,1,'the goblin cleric'),
('Haze',8,1,'Haze'),
('Erasamus',2,1,'a pirahna'),
('Petrale',16,1,'a large brown bear'),
('Gnervil',3,0,'no one'),
('Seras',8,0,'no one'),
('Mung',20,0,'no one'),
('Gerard',19,1,'a dragon whelp'),
('EeizeJr',8,1,'a <g>gnome chef\'s assistant<z>'),
('Toranaga',3,0,'no one'),
('Greyshoot',6,1,'Acidbear'),
('Planhigion',3,1,'an aspiring young shaman'),
('Tywydd',11,1,'<g>an archer<1>'),
('Covad',2,0,'no one'),
('Meddwl',5,1,'a male citizen'),
('Slyder',13,0,'no one'),
('StaggerLoo',3,0,'no one'),
('Amihere',3,0,'no one'),
('KittyKitty',4,0,'no one'),
('Phoofur',21,1,'a <r>wild elf archer<z>'),
('Thac',2,0,'no one'),
('Pure',3,0,'no one'),
('Slideth',18,1,'a pygmy bone-woman'),
('Rynth',20,0,'no one'),
('Balzaar',12,1,'<r>a battle-mage<1>'),
('Ice',4,0,'no one'),
('Darchri',3,0,'no one'),
('Vacra',2,1,'Vacra'),
('Ambpot',19,1,'Mr. McGnee'),
('Rero',17,0,'no one'),
('Ghovinda',4,1,'a <C>gnomish houngan<1>'),
('Sindarin',2,0,'no one'),
('Araxus',50,0,'no one'),
('Triz',3,1,'a draft horse'),
('Fondu',6,0,'no one'),
('Hilech',14,0,'no one'),
('GauchoX',3,0,'no one'),
('TigerPosture',1,1,'a <B>city watchman<1>'),
('Madeleine',3,0,'no one'),
('Leer',6,0,'no one'),
('Brandon',6,1,'Nicodemus the old fisherman'),
('Nyr',3,0,'no one'),
('Katrina',3,0,'no one'),
('Mendos',2,0,'no one'),
('Ranko',2,0,'no one'),
('SexyBoy',4,0,'no one'),
('Material',15,0,'no one'),
('PermaSupernaut',4,1,'PermaSupernaut'),
('Thingol',2,1,'Peel'),
('Adlion',2,1,'a Grimhaven youth'),
('Matilda',5,1,'a robber'),
('Leafy',2,0,'no one'),
('Tigerposture',11,1,'a young student'),
('TheMidasArray',16,1,'the <g>wardrobe mistress<z>'),
('Crack',6,1,'Jelly'),
('Kell',2,1,'an <R>obedient zombie<1>'),
('Gmoney',2,0,'no one'),
('Chohhey',2,0,'no one'),
('Erige',5,1,'a gnome farmer'),
('Shlade',2,1,'a cleric'),
('MajinBoo',5,0,'no one'),
('Kunundra',3,0,'no one'),
('PilsburyDoughboy',3,1,'an <R>obedient zombie<1>'),
('Doughboy',3,1,'a <r>lady-of-the-evening<1>'),
('Kitado',21,1,'Kitado'),
('Danish',3,0,'no one'),
('Jin',3,1,'an <R>elite sentry<1>'),
('Roothopper',7,1,'<o>a grimey janitor<z>'),
('Anderdr',4,1,'an elderly woman'),
('Salina',5,1,'<o>a homeless man<z>'),
('HuoLee',3,1,'an old broken down horse'),
('Nilla',15,1,'Delgado'),
('Baozhi',2,1,'a cleric'),
('Nicholas',11,0,'no one'),
('Cranestyle',2,0,'no one'),
('SodaCroft',4,0,'no one'),
('Smokums',2,0,'no one'),
('Kytanial',5,1,'an <R>obedient zombie<1>'),
('CaptainCum',2,1,'Bump'),
('Sifting',3,0,'no one'),
('Stampede',11,1,'Master Kretek'),
('Garlic',11,0,'no one'),
('Clashofthetitans',3,0,'no one'),
('Artere',2,0,'no one'),
('Multiplay',4,0,'no one'),
('Onion',12,1,'a hobgoblin cook'),
('Beany',2,0,'no one'),
('Sheisterhausen',5,0,'no one'),
('Woob',3,0,'no one'),
('Privet',9,1,'the head priest'),
('Delusion',6,0,'no one'),
('Frolic',9,1,'an actor'),
('Delirium',15,0,'no one'),
('Eclipse',2,0,'no one'),
('Sammy',17,1,'a drow elite guard'),
('Morkandar',4,0,'no one'),
('Bick',4,0,'no one'),
('MissFirer',5,0,'no one'),
('Gunther',13,0,'no one'),
('DuGneisse',2,0,'no one'),
('DuGneiss',9,0,'no one'),
('Ablut',2,0,'no one'),
('Hammerhead',21,1,'a crazed gladiator'),
('Elenore',6,0,'no one'),
('Antrax',6,0,'no one'),
('Alona',2,1,'a sparrow'),
('Straep',7,1,'a trolloc'),
('Possum',2,0,'no one'),
('Deunan',2,0,'no one'),
('Thoran',4,0,'no one'),
('Ahoy',2,0,'no one'),
('Beth',32,1,'Leroy, the King of Grimhaven'),
('Quezlar',2,1,'a male citizen'),
('Perma',50,0,'no one'),
('Shylewryn',36,0,'no one'),
('Sodium',2,1,'a female citizen'),
('DarkRemains',35,0,'no one'),
('Thuahthstle',24,1,'a sparrow'),
('DarkDooDoo',2,1,'a <C>gnomish houngan<1>'),
('Thief',3,0,'no one'),
('Kryanna',3,1,'a <B>city watchman<1>'),
('Dolore',4,0,'no one'),
('ZagZig',3,1,'Nicodemus the old fisherman'),
('Beesque',5,0,'no one'),
('GoofusMcCan',3,0,'no one'),
('Vurd',2,1,'an <R>obedient zombie<1>'),
('Church',27,1,'Church'),
('Rohind',2,0,'no one'),
('Dumbledore',25,0,'no one'),
('Xarqwa',3,1,'Nicodemus the old fisherman'),
('PigNutz',8,1,'a gnome outpost sentry'),
('Hiestand',4,0,'no one'),
('Cali',5,0,'no one'),
('Talsheim',3,1,'a hawk'),
('Mudbutt',36,0,'no one'),
('Losis',2,0,'no one'),
('Crushiating',4,0,'no one'),
('Drieso',2,0,'no one'),
('Lokri',13,0,'no one'),
('Fele',29,1,'Fele'),
('String',41,1,'<o>a dirty refuse hauler<1>'),
('Blaine',5,0,'no one'),
('Antoine',5,0,'no one'),
('Duenan',1,1,'a dungeon guard'),
('Leromir',3,1,'a paladin leper hunter'),
('Smackdown',3,0,'no one'),
('Baradic',2,0,'no one'),
('Barrian',25,1,'the siren'),
('Freedom',2,0,'no one'),
('Cake',36,0,'no one'),
('Shrub',36,1,'<w>a giant slug<1>'),
('Saell',33,1,'Kryvrkian'),
('Fame',25,1,'a paladin lieutenant'),
('Todsheim',3,1,'a hawk'),
('Moroven',5,0,'no one'),
('Meber',5,0,'no one'),
('Mannix',24,0,'no one'),
('Tsetse',23,0,'no one'),
('Rhomak',10,1,'Evain'),
('Prostav',2,0,'no one'),
('Forge',2,0,'no one'),
('Calia',37,1,'a <r>wild elf<z>'),
('Twinge',8,1,'a gnome scout'),
('Dontdie',37,1,'a spotted lion'),
('Swingline',3,1,'a robber'),
('Tallericky',7,0,'no one'),
('Cascade',7,1,'the hobbit embassy receptionist'),
('Fiddeck',17,0,'no one'),
('River',27,0,'no one'),
('Logobola',31,1,'<o>a drifter<z>'),
('Jaqen',2,0,'no one'),
('Dinkleburg',11,0,'no one'),
('Rhaego',10,0,'no one'),
('Steve Irwin',60,1,'a stingray'),
('Epithet',13,1,'Epithet'),
('Aec',2,0,'no one'),
('Aed',2,0,'no one'),
('Thedarkone',20,1,'an <B>elite cityguard<1>'),
('Feralas',10,0,'no one'),
('Urizen',16,0,'no one'),
('Ijaz',2,1,'a small crow'),
('Bhutz',4,1,'a <y>gnome ant-rider<z>'),
('Amadeus',5,0,'no one'),
('Basshunter',5,0,'no one'),
('Sog',5,0,'no one'),
('Victor',6,1,'Nicodemus the old fisherman'),
('Snax',5,0,'no one'),
('Bloodrayne',5,0,'no one'),
('Anklebiter',6,0,'no one'),
('Vampyre',15,1,'Vampyre'),
('Whole',17,1,'Whole'),
('Bhut',3,0,'no one'),
('Morpheus',14,1,'<o>Overlord Toreth<z>'),
('Sihx',27,0,'no one'),
('Kelranth',22,1,'Kelranth'),
('Charles',12,0,'no one'),
('Enzyme',5,1,'an apprentice shaman'),
('Etanis',2,1,'a hawk'),
('Emma',44,0,'no one'),
('Dagoth',9,0,'no one'),
('Tic',38,1,'the head mage technician'),
('Yalp',10,0,'no one'),
('Phalthar',24,1,'no one'),
('Jaxom',40,1,'a dagger thrower'),
('Gadget',3,0,'no one'),
('Mellancor',2,0,'no one'),
('Brook',6,0,'no one'),
('Lumpy',30,1,'a spotted lion'),
('Clock',5,0,'no one'),
('Precious',8,0,'no one'),
('Timon',14,0,'no one'),
('Bender',10,1,'an elven traveler'),
('Bromidrosis',17,1,'Gnarrelus, the gadget-maker'),
('Zigma',28,1,'a caravan guard'),
('Ceirseldju',20,0,'no one'),
('Beef',20,0,'no one'),
('Edema',12,0,'no one'),
('Snow',17,1,'Snow'),
('Gruntle',12,0,'no one'),
('Chaynal',31,0,'no one'),
('Mortal',4,0,'no one'),
('Colambo',7,0,'no one'),
('Stark',20,0,'no one'),
('Arya',21,0,'no one'),
('Aquaman',4,1,'Nicodemus the old fisherman'),
('Permidaeth',50,0,'no one'),
('Minotauro',3,0,'no one'),
('Angus',50,0,'no one'),
('Baal',50,0,'no one'),
('Permanaut',20,1,'an undead savage warrior'),
('Urraca',15,0,'no one');
/*!40000 ALTER TABLE `permadeath` ENABLE KEYS */;
UNLOCK TABLES;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2024-07-02 15:14:43
