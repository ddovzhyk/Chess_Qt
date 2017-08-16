#pragma once
#include <memory>
#include <QAbstractListModel>

class Logic: public QAbstractListModel
{
    Q_OBJECT

public:
    enum GlobalConstants {
        BOARD_SIZE = 8,
        NUMBER_FIGURES = 32,

        TYPE_WHITE_ROOK = 0,
        TYPE_BLACK_ROOK = 1,
        TYPE_WHITE_KNIGHT = 2,
        TYPE_BLACK_KNIGHT = 3,
        TYPE_WHITE_BISHOP = 4,
        TYPE_BLACK_BISHOP = 5,
        TYPE_WHITE_QUEEN = 6,
        TYPE_BLACK_QUEEN = 7,
        TYPE_WHITE_KING = 8,
        TYPE_BLACK_KING = 9,
        TYPE_WHITE_PAWN = 10,
        TYPE_BLACK_PAWN = 11,

        INDEX_KING_WHITE = 14,
        INDEX_KING_BLACK = 15
    };

    enum Roles {
        Type = Qt::UserRole,
        PositionX,
        PositionY,
    };
    
    struct Cell
    {
        int x;
        int y;
    };

    struct Move
    {
        int index;
        int fromX;
        int fromY;
        int toX;
        int toY;
    };

public:
    explicit Logic(QObject *parent = 0);
    ~Logic();

    Q_PROPERTY(int boardSize READ boardSize CONSTANT)
    int boardSize() const;

    Q_INVOKABLE void clear();
    Q_INVOKABLE bool move(int fromX, int fromY, int toX, int toY);
    void setStartFigures();
    bool isTrueCell(int x, int y);
    bool isFreeCell(int x, int y);
    bool isEnemy(int forIndex, int toX, int toY);
    bool isWhiteFigure(int type);
    QList<Cell> getPossibleMoves(int index, int fromX, int fromY);
    QList<Cell> addMovesRook(int index, int fromX, int fromY);
    QList<Cell> addMovesKnight(int index, int fromX, int fromY);
    QList<Cell> addMovesBishop(int index, int fromX, int fromY);
    QList<Cell> addMovesQueen(int index, int fromX, int fromY);
    QList<Cell> addMovesKing(int index, int fromX, int fromY);
    QList<Cell> addMovesPawn(int index, int fromX, int fromY);
    void addMoveToHistory(int index, int fromX, int fromY, int toX, int toY);
    Q_INVOKABLE int getCurrentTurn();
    Q_INVOKABLE bool isCorrectTurn(int type);
    Q_INVOKABLE bool isDeadKingWhite();
    Q_INVOKABLE bool isDeadKingBlack();
    Q_INVOKABLE void saveHistory();
    Q_INVOKABLE void newGame();
    Q_INVOKABLE void loadGame();
    Q_INVOKABLE int getIndexHistory();
    Q_INVOKABLE void prevMove();
    Q_INVOKABLE void nextMove();
    Q_INVOKABLE bool isNewestMove();
    Q_INVOKABLE void eraseHistory();
    Q_INVOKABLE bool isCorrectSavedGame();

protected:
    int rowCount(const QModelIndex & parent) const override;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

private:
    struct Impl;
    std::unique_ptr<Impl> impl;
    int currentTurn = 0;
    QList<Move> history;
    int indexHistory = -1;
};
