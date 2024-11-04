-- SQLite schema for chess openings

-- Create tables with appropriate types
-- Note: SQLite doesn't have a separate INT type, it uses INTEGER

CREATE TABLE IF NOT EXISTS fen_italian (
    italianid INTEGER PRIMARY KEY,
    row INTEGER,
    fen_notation TEXT
);

CREATE TABLE IF NOT EXISTS fen_queen_gambit (
    queengambitid INTEGER PRIMARY KEY,
    row INTEGER,
    fen_notation TEXT
);

CREATE TABLE IF NOT EXISTS fen_ruy_lopez (
    ruylopezid INTEGER PRIMARY KEY,
    row INTEGER,
    fen_notation TEXT
);

-- Insert data into fen_italian
INSERT INTO fen_italian (italianid, row, fen_notation) VALUES
    (1200, 1, 'r1bqkbnr'),
    (1201, 2, 'pppp1ppp'),
    (1202, 3, '2n5'),
    (1203, 4, '4p3'),
    (1204, 5, '2B1P3'),
    (1205, 6, '5N2'),
    (1206, 7, 'PPPP1PPP'),
    (1207, 8, 'RNBQK2R');

-- Insert data into fen_queen_gambit
INSERT INTO fen_queen_gambit (queengambitid, row, fen_notation) VALUES
    (1000, 1, 'rnbqkb1r'),
    (1001, 2, 'ppp2ppp'),
    (1002, 3, '4pn2'),
    (1003, 4, '3p4'),
    (1004, 5, '2PP4'),
    (1005, 6, '5N2'),
    (1006, 7, 'PP2PPPP'),
    (1007, 8, 'RNBQKB1R');

-- Insert data into fen_ruy_lopez
INSERT INTO fen_ruy_lopez (ruylopezid, row, fen_notation) VALUES
    (1100, 1, 'r1bqkbnr'),
    (1101, 2, 'pppp1ppp'),
    (1102, 3, '2n5'),
    (1103, 4, '1B2p3'),
    (1104, 5, '4P3'),
    (1105, 6, '5N2'),
    (1106, 7, 'PPPP1PPP'),
    (1107, 8, 'RNBQK2R');

-- Create indices for better query performance
CREATE INDEX IF NOT EXISTS idx_italian_row ON fen_italian(row);
CREATE INDEX IF NOT EXISTS idx_queen_gambit_row ON fen_queen_gambit(row);
CREATE INDEX IF NOT EXISTS idx_ruy_lopez_row ON fen_ruy_lopez(row);