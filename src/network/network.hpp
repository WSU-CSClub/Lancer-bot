#pragma once
#include <sqlite3.h>
#include <iostream>
#include <stdexcept>


//All member variables contain m_ htis sohudl be the standard conention going forward

class ChessEngineDB
{

    private:
    sqlite3* m_db;


    void CheckError(int result,const char* operation){
        if(result != SQLITE_OK){
            const char* error = sqlite3_errmsg(m_db);
            
            throw std::runtime_error("ERROR " + std::string(operation) + " :" + error);

        }
    }

    public: 

    struct Position {
        int id;
        int row;
        std::string fen_notation;
    };

    ChessEngineDB(const std::string& filename){
        int result = sqlite3_open(filename.c_str(),&m_db);
        CheckError(result,"Opening database");

    }

    ~ChessEngineDB() {
        if (m_db) {
            sqlite3_close(m_db);
        }
    }

        std::string getCompleteFEN(const std::vector<Position>& positions) {
        std::string complete_fen;
        // Sort positions by row
        std::vector<std::string> rows(8);
        
        for (const auto& pos : positions) {
            rows[pos.row - 1] = pos.fen_notation;
        }
        
        // Join rows with slashes to create complete FEN
        for (size_t i = 0; i < rows.size(); ++i) {
            complete_fen += rows[i];
            if (i < rows.size() - 1) {
                complete_fen += "/";
            }
        }
        
        // Add additional FEN information (side to move, castling, etc.)
        complete_fen += " w KQkq - 0 1";
        return complete_fen;
    }


    std::vector<Position> GetChessBoardPosition(){
        return GetPosition("chess_openings","opening_id");
    }
    std::vector<Position> getItalianPosition() {
        return GetPosition("fen_italian", "italianid");
    }
    std::vector<Position> GetQueensGambitPosition(){
        return GetPosition("fen_queen_gambit","queengambitid");
    }
    private:
    std::vector<Position> GetPosition(const std::string& table, const std::string& id_column){
        std::vector<Position> position;
        sqlite3_stmt* stmt;
        
        std::string sql = "SELECT " + id_column + ", row, fen_notation FROM " + table + 
                         " ORDER BY row";

        int result = sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr);
        CheckError(result, "preparing statement");

        while(sqlite3_step(stmt) == SQLITE_ROW){
            Position pos;
            pos.id = sqlite3_column_int(stmt,0);
            pos.row = sqlite3_column_int(stmt,1);
            pos.fen_notation = reinterpret_cast<const char*>(sqlite3_column_text(stmt,2));
            position.push_back(pos);
        }
        sqlite3_finalize(stmt);
        return position;
    }
};