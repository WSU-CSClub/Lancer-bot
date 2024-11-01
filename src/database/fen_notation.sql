-- MySQL dump 10.13  Distrib 8.0.31, for macos12 (x86_64)
--
-- Host: localhost    Database: chess_fennotations
-- ------------------------------------------------------
-- Server version	8.0.31

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!50503 SET NAMES utf8mb4 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

-- -----------------------------------------------------
-- Schema chess_fennotations
-- -----------------------------------------------------
CREATE SCHEMA IF NOT EXISTS `chess_fennotations` DEFAULT CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci ;
USE `chess_fennotations` ;

--
-- Table structure for table `fen_italian`
--

DROP TABLE IF EXISTS `fen_italian`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `fen_italian` (
  `italianid` int NOT NULL,
  `row` int DEFAULT NULL,
  `fen_notation` varchar(45) DEFAULT NULL,
  PRIMARY KEY (`italianid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `fen_italian`
--

LOCK TABLES `fen_italian` WRITE;
/*!40000 ALTER TABLE `fen_italian` DISABLE KEYS */;
INSERT INTO `fen_italian` VALUES (1200,1,'r1bqkbnr'),(1201,2,'pppp1ppp'),(1202,3,'2n5'),(1203,4,'4p3'),(1204,5,'2B1P3'),(1205,6,'5N2'),(1206,7,'PPPP1PPP'),(1207,8,'RNBQK2R');
/*!40000 ALTER TABLE `fen_italian` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `fen_queen_gambit`
--

DROP TABLE IF EXISTS `fen_queen_gambit`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `fen_queen_gambit` (
  `queengambitid` int NOT NULL,
  `row` int DEFAULT NULL,
  `fen_notation` varchar(45) DEFAULT NULL,
  PRIMARY KEY (`queengambitid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `fen_queen_gambit`
--

LOCK TABLES `fen_queen_gambit` WRITE;
/*!40000 ALTER TABLE `fen_queen_gambit` DISABLE KEYS */;
INSERT INTO `fen_queen_gambit` VALUES (1000,1,'rnbqkb1r'),(1001,2,'ppp2ppp'),(1002,3,'4pn2'),(1003,4,'3p4'),(1004,5,'2PP4'),(1005,6,'5N2'),(1006,7,'PP2PPPP'),(1007,8,'RNBQKB1R');
/*!40000 ALTER TABLE `fen_queen_gambit` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `fen_ruy_lopez`
--

DROP TABLE IF EXISTS `fen_ruy_lopez`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `fen_ruy_lopez` (
  `ruylopezid` int NOT NULL,
  `row` int DEFAULT NULL,
  `fen_notation` varchar(45) DEFAULT NULL,
  PRIMARY KEY (`ruylopezid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `fen_ruy_lopez`
--

LOCK TABLES `fen_ruy_lopez` WRITE;
/*!40000 ALTER TABLE `fen_ruy_lopez` DISABLE KEYS */;
INSERT INTO `fen_ruy_lopez` VALUES (1100,1,'r1bqkbnr'),(1101,2,'pppp1ppp'),(1102,3,'2n5'),(1103,4,'1B2p3'),(1104,5,'4P3'),(1105,6,'5N2'),(1106,7,'PPPP1PPP'),(1107,8,'RNBQK2R');
/*!40000 ALTER TABLE `fen_ruy_lopez` ENABLE KEYS */;
UNLOCK TABLES;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2024-10-31 22:36:04
