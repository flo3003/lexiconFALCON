-- phpMyAdmin SQL Dump
-- version 4.6.6deb4
-- https://www.phpmyadmin.net/
--
-- Φιλοξενητής: localhost:3306
-- Χρόνος δημιουργίας: 21 Αυγ 2019 στις 20:48:03
-- Έκδοση διακομιστή: 5.7.24
-- Έκδοση PHP: 7.0.33-0+deb9u3

SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8mb4 */;

--
-- Βάση δεδομένων: `LexiconFALCON_db`
--

CREATE DATABASE IF NOT EXISTS LexiconFALCON_db;

USE LexiconFALCON_db;

-- --------------------------------------------------------

--
-- Δομή πίνακα για τον πίνακα `coo_matrix`
--

CREATE TABLE `coo_matrix` (
  `word_a` int(11) NOT NULL,
  `word_b` int(11) NOT NULL,
  `cooccurrence` double NOT NULL,
  `log_cooccurrence` double NOT NULL
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

-- --------------------------------------------------------

--
-- Δομή πίνακα για τον πίνακα `framenet`
--

CREATE TABLE `framenet` (
  `source_id` int(11) NOT NULL,
  `target_id` int(11) NOT NULL,
  `semantic_value` int(11) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- --------------------------------------------------------

--
-- Δομή πίνακα για τον πίνακα `ppdb`
--

CREATE TABLE `ppdb` (
  `source_id` int(11) NOT NULL,
  `target_id` int(11) NOT NULL,
  `semantic_value` int(11) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- --------------------------------------------------------

--
-- Δομή πίνακα για τον πίνακα `wordnet`
--

CREATE TABLE `wordnet` (
  `source_id` int(11) NOT NULL,
  `target_id` int(11) NOT NULL,
  `semantic_value` int(11) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- --------------------------------------------------------

--
-- Δομή πίνακα για τον πίνακα `word_mapping`
--

CREATE TABLE `word_mapping` (
  `word_id` int(11) NOT NULL,
  `name` varchar(100) CHARACTER SET latin1 NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

--
-- Ευρετήρια για άχρηστους πίνακες
--

--
-- Ευρετήρια για πίνακα `coo_matrix`
--
ALTER TABLE `coo_matrix`
  ADD KEY `user_id` (`word_a`),
  ADD KEY `item_id` (`word_b`);

--
-- Ευρετήρια για πίνακα `framenet`
--
ALTER TABLE `framenet`
  ADD PRIMARY KEY (`source_id`,`target_id`);

--
-- Ευρετήρια για πίνακα `ppdb`
--
ALTER TABLE `ppdb`
  ADD PRIMARY KEY (`source_id`,`target_id`);

--
-- Ευρετήρια για πίνακα `wordnet`
--
ALTER TABLE `wordnet`
  ADD PRIMARY KEY (`source_id`,`target_id`);

--
-- Ευρετήρια για πίνακα `word_mapping`
--
ALTER TABLE `word_mapping`
  ADD PRIMARY KEY (`word_id`);

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
