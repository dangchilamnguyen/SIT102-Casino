-- phpMyAdmin SQL Dump
-- version 5.2.1
-- https://www.phpmyadmin.net/
--
-- Host: 127.0.0.1
-- Generation Time: Sep 23, 2026 at 02:29 AM
-- Server version: 10.4.32-MariaDB
-- PHP Version: 8.2.12

SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
START TRANSACTION;
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8mb4 */;

--
-- Database: `casino sit102`
--

-- --------------------------------------------------------

--
-- Table structure for table `game_result`
--

CREATE TABLE `game_result` (
  `MATCH_ID` int(11) NOT NULL,
  `PLAYER_ID` int(11) NOT NULL,
  `GAME_TYPE` varchar(30) NOT NULL,
  `RESULT` varchar(10) NOT NULL,
  `CHIPS_CHANGE` int(11) NOT NULL DEFAULT 0,
  `PLAYED_TIME` datetime NOT NULL DEFAULT current_timestamp()
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Dumping data for table `game_result`
--

INSERT INTO `game_result` (`MATCH_ID`, `PLAYER_ID`, `GAME_TYPE`, `RESULT`, `CHIPS_CHANGE`, `PLAYED_TIME`) VALUES
(1, 1, 'Blackjack', 'Win', 100, '2026-09-23 10:21:41'),
(2, 1, 'Dice', 'Lose', -50, '2026-09-23 10:21:41'),
(3, 2, 'Blackjack', 'Win', 200, '2026-09-23 10:21:41'),
(4, 3, 'Dice', 'Draw', 0, '2026-09-23 10:21:41');

-- --------------------------------------------------------

--
-- Table structure for table `player`
--

CREATE TABLE `player` (
  `PLAYER_ID` int(11) NOT NULL,
  `NAME` text NOT NULL,
  `CHIPS` int(11) NOT NULL DEFAULT 0,
  `GAME_PLAYED` int(11) NOT NULL DEFAULT 0,
  `GAMES_WON` int(11) NOT NULL DEFAULT 0,
  `TOTAL_WINS` int(11) NOT NULL DEFAULT 0
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Dumping data for table `player`
--

INSERT INTO `player` (`PLAYER_ID`, `NAME`, `CHIPS`, `GAME_PLAYED`, `GAMES_WON`, `TOTAL_WINS`) VALUES
(1, 'Lam', 1000, 10, 7, 1200),
(2, 'Minh', 800, 8, 4, 900),
(3, 'An', 600, 7, 3, 700),
(4, 'Huy', 1500, 12, 9, 1800);

--
-- Indexes for dumped tables
--

--
-- Indexes for table `game_result`
--
ALTER TABLE `game_result`
  ADD PRIMARY KEY (`MATCH_ID`),
  ADD KEY `PLAYER_ID` (`PLAYER_ID`);

--
-- Indexes for table `player`
--
ALTER TABLE `player`
  ADD PRIMARY KEY (`PLAYER_ID`);

--
-- AUTO_INCREMENT for dumped tables
--

--
-- AUTO_INCREMENT for table `game_result`
--
ALTER TABLE `game_result`
  MODIFY `MATCH_ID` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=5;

--
-- AUTO_INCREMENT for table `player`
--
ALTER TABLE `player`
  MODIFY `PLAYER_ID` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=5;

--
-- Constraints for dumped tables
--

--
-- Constraints for table `game_result`
--
ALTER TABLE `game_result`
  ADD CONSTRAINT `game_result_ibfk_1` FOREIGN KEY (`PLAYER_ID`) REFERENCES `player` (`PLAYER_ID`);
COMMIT;

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
