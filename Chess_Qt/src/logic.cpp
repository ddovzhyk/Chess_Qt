#include "logic.h"
#include <QList>
#include <QByteArray>
#include <QHash>
#include <iostream>
#include <QFile>
#include <QTextStream>
#include <QDir>

struct Figure
{
    int type;
    int x;
    int y;
};
struct Logic::Impl
{
    QList<Figure> figures;

    int findByPosition(int x, int y);
};

int Logic::Impl::findByPosition(int x, int y)
{
    for (int i(0); i<figures.size(); ++i) {
        if (figures[i].x != x || figures[i].y != y ) {
            continue;
        }
        return i;
    }
    return -1;
}

Logic::Logic(QObject *parent)
    : QAbstractListModel(parent)
    , impl(new Impl())
{
    for (int i = 0; i < NUMBER_FIGURES; ++i)
        impl->figures << Figure();
    setStartFigures();
}

Logic::~Logic()
{
    if (!impl->figures.empty())
        impl->figures.clear();
    if (!history.empty())
        history.clear();
}

int Logic::boardSize() const
{
    return BOARD_SIZE;
}

int Logic::rowCount(const QModelIndex & ) const
{
    return impl->figures.size();
}
QHash<int, QByteArray> Logic::roleNames() const
{
    QHash<int, QByteArray> names;

    names.insert(Roles::Type      , "type");
    names.insert(Roles::PositionX , "positionX");
    names.insert(Roles::PositionY , "positionY");

    return names;
}
QVariant Logic::data(const QModelIndex & modelIndex, int role) const
{
    if (!modelIndex.isValid()) {
        return QVariant();
    }

    int index = static_cast<int>(modelIndex.row());

    if (index >= impl->figures.size() || index < 0) {
        return QVariant();
    }

    Figure & figure = impl->figures[index];

    switch (role) {
    case Roles::Type     : return figure.type;
    case Roles::PositionX: return figure.x;
    case Roles::PositionY: return figure.y;
    }
    return QVariant();
}

void Logic::clear()
{
    beginResetModel();
    impl->figures.clear();
    endResetModel();
}

bool Logic::move(int fromX, int fromY, int toX, int toY)
{
    QModelIndex topLeft;
    QModelIndex bottomRight;
    QList <Cell> possibleMoves;
    int index;
    bool result = false;

    index = impl->findByPosition(fromX, fromY);
    if (index < 0)
        return false;
    impl->figures[index].x = fromX;
    impl->figures[index].y = fromY;

    if (isTrueCell(toX, toY))
        if (isCorrectTurn(impl->figures[index].type))
        {
            possibleMoves = getPossibleMoves(index, fromX, fromY);
            if (!possibleMoves.isEmpty())
                for (auto it = possibleMoves.begin();
                        it != possibleMoves.end(); ++it)
                    if (it->x == toX && it->y == toY)
                    {
                        if (indexHistory != history.size() - 1)
                            eraseHistory();
                        if (isEnemy(index, toX, toY))
                        {
                            int index_enemy = impl->findByPosition(toX, toY);
                            impl->figures[index_enemy].x = BOARD_SIZE;
                            impl->figures[index_enemy].y = BOARD_SIZE;
                            addMoveToHistory(index_enemy, toX, toY,
                                             BOARD_SIZE, BOARD_SIZE);
                        }
                        impl->figures[index].x = toX;
                        impl->figures[index].y = toY;
                        currentTurn = !currentTurn;
                        addMoveToHistory(index, fromX, fromY, toX, toY);
                        result = true;
                        break;
                    }
        }
    topLeft = createIndex(0, 0);
    bottomRight = createIndex(NUMBER_FIGURES - 1, 0);
    emit dataChanged(topLeft, bottomRight);
    if (!possibleMoves.isEmpty())
        possibleMoves.clear();
    return result;
}

void Logic::setStartFigures()
{
    impl->figures[0] = Figure{ TYPE_WHITE_ROOK, 0, 7 };
    impl->figures[1] = Figure { TYPE_WHITE_ROOK, 7, 7 };
    impl->figures[2] = Figure { TYPE_BLACK_ROOK, 0, 0 };
    impl->figures[3] = Figure { TYPE_BLACK_ROOK, 7, 0 };

    impl->figures[4] = Figure { TYPE_WHITE_KNIGHT, 1, 7 };
    impl->figures[5] = Figure { TYPE_WHITE_KNIGHT, 6, 7 };
    impl->figures[6] = Figure { TYPE_BLACK_KNIGHT, 1, 0 };
    impl->figures[7] = Figure { TYPE_BLACK_KNIGHT, 6, 0 };

    impl->figures[8] = Figure { TYPE_WHITE_BISHOP, 2, 7 };
    impl->figures[9] = Figure { TYPE_WHITE_BISHOP, 5, 7 };
    impl->figures[10] = Figure { TYPE_BLACK_BISHOP, 2, 0 };
    impl->figures[11] = Figure { TYPE_BLACK_BISHOP, 5, 0 };

    impl->figures[12] = Figure { TYPE_WHITE_QUEEN, 3, 7 };
    impl->figures[13] = Figure { TYPE_BLACK_QUEEN, 3, 0 };

    impl->figures[14] = Figure { TYPE_WHITE_KING, 4, 7 };
    impl->figures[15] = Figure { TYPE_BLACK_KING, 4, 0 };

    impl->figures[16] = Figure { TYPE_WHITE_PAWN, 0, 6 };
    impl->figures[17] = Figure { TYPE_BLACK_PAWN, 0, 1 };
    impl->figures[18] = Figure { TYPE_WHITE_PAWN, 1, 6 };
    impl->figures[19] = Figure { TYPE_BLACK_PAWN, 1, 1 };
    impl->figures[20] = Figure { TYPE_WHITE_PAWN, 2, 6 };
    impl->figures[21] = Figure { TYPE_BLACK_PAWN, 2, 1 };
    impl->figures[22] = Figure { TYPE_WHITE_PAWN, 3, 6 };
    impl->figures[23] = Figure { TYPE_BLACK_PAWN, 3, 1 };
    impl->figures[24] = Figure { TYPE_WHITE_PAWN, 4, 6 };
    impl->figures[25] = Figure { TYPE_BLACK_PAWN, 4, 1 };
    impl->figures[26] = Figure { TYPE_WHITE_PAWN, 5, 6 };
    impl->figures[27] = Figure { TYPE_BLACK_PAWN, 5, 1 };
    impl->figures[28] = Figure { TYPE_WHITE_PAWN, 6, 6 };
    impl->figures[29] = Figure { TYPE_BLACK_PAWN, 6, 1 };
    impl->figures[30] = Figure { TYPE_WHITE_PAWN, 7, 6 };
    impl->figures[31] = Figure { TYPE_BLACK_PAWN, 7, 1 };
}

bool Logic:: isTrueCell(int x, int y)
{
    return x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE;
}

bool Logic:: isFreeCell(int x, int y)
{
    return impl->findByPosition(x, y) == -1;
}

bool Logic:: isEnemy(int forIndex, int toX, int toY)
{
    int toIndex;

    toIndex = impl->findByPosition(toX, toY);
    if (toIndex == -1)
        return false;
    return isWhiteFigure(impl->figures[forIndex].type) ==
            !isWhiteFigure(impl->figures[toIndex].type);
}

bool Logic:: isWhiteFigure(int type)
{
    return type % 2 == 0;
}

QList<Logic::Cell> Logic::getPossibleMoves(int index, int fromX, int fromY)
{
    switch (impl->figures[index].type)
    {
        case TYPE_WHITE_ROOK :
        case TYPE_BLACK_ROOK :
            return addMovesRook(index, fromX, fromY);
        case TYPE_WHITE_KNIGHT :
        case TYPE_BLACK_KNIGHT :
            return addMovesKnight(index, fromX, fromY);
        case TYPE_WHITE_BISHOP :
        case TYPE_BLACK_BISHOP :
            return addMovesBishop(index, fromX, fromY);
        case TYPE_WHITE_QUEEN :
        case TYPE_BLACK_QUEEN :
            return addMovesQueen(index, fromX, fromY);
        case TYPE_WHITE_KING :
        case TYPE_BLACK_KING :
            return addMovesKing(index, fromX, fromY);
        case TYPE_WHITE_PAWN :
        case TYPE_BLACK_PAWN :
            return addMovesPawn(index, fromX, fromY);
    }
    return QList<Cell>();
}

QList<Logic::Cell> Logic::addMovesRook(int index, int fromX, int fromY)
{
    QList<Cell> possMoves;
    int i;

    i = 0;
    while (++i && isTrueCell(fromX, fromY - i) && isFreeCell(fromX, fromY - i))
        possMoves << Cell { fromX, fromY - i };
    if (isTrueCell(fromX, fromY - i) && isEnemy(index, fromX, fromY - i))
        possMoves << Cell { fromX, fromY - i };

    i = 0;
    while (++i && isTrueCell(fromX, fromY + i) && isFreeCell(fromX, fromY + i))
        possMoves << Cell { fromX, fromY + i };
    if (isTrueCell(fromX, fromY + i) && isEnemy(index, fromX, fromY + i))
        possMoves << Cell { fromX, fromY + i };

    i = 0;
    while (++i && isTrueCell(fromX - i, fromY) && isFreeCell(fromX - i, fromY))
        possMoves << Cell { fromX - i, fromY };
    if (isTrueCell(fromX - i, fromY) && isEnemy(index, fromX - i, fromY))
        possMoves << Cell { fromX - i, fromY };

    i = 0;
    while (++i && isTrueCell(fromX + i, fromY) && isFreeCell(fromX + i, fromY))
        possMoves << Cell { fromX + i, fromY };
    if (isTrueCell(fromX + i, fromY) && isEnemy(index, fromX + i, fromY))
        possMoves << Cell { fromX + i, fromY };

    return possMoves;
}

QList<Logic::Cell> Logic::addMovesKnight(int index, int fromX, int fromY)
{
    QList<Cell> possMoves;

    if (isTrueCell(fromX - 2, fromY - 1) &&
            (isFreeCell(fromX - 2, fromY - 1) || isEnemy(index, fromX - 2, fromY - 1)))
        possMoves << Cell { fromX - 2, fromY - 1 };

    if (isTrueCell(fromX - 2, fromY + 1) &&
            (isFreeCell(fromX - 2, fromY + 1) || isEnemy(index, fromX - 2, fromY + 1)))
        possMoves << Cell { fromX - 2, fromY + 1 };

    if (isTrueCell(fromX - 1, fromY - 2) &&
            (isFreeCell(fromX - 1, fromY - 2) || isEnemy(index, fromX - 1, fromY - 2)))
        possMoves << Cell { fromX - 1, fromY - 2 };

    if (isTrueCell(fromX - 1, fromY + 2) &&
            (isFreeCell(fromX - 1, fromY + 2) || isEnemy(index, fromX - 1, fromY + 2)))
        possMoves << Cell { fromX - 1, fromY + 2 };

    if (isTrueCell(fromX + 1, fromY - 2) &&
            (isFreeCell(fromX + 1, fromY - 2) || isEnemy(index, fromX + 1, fromY - 2)))
        possMoves << Cell { fromX + 1, fromY - 2 };

    if (isTrueCell(fromX + 1, fromY + 2) &&
            (isFreeCell(fromX + 1, fromY + 2) || isEnemy(index, fromX + 1, fromY + 2)))
        possMoves << Cell { fromX + 1, fromY + 2 };

    if (isTrueCell(fromX + 2, fromY - 1) &&
            (isFreeCell(fromX + 2, fromY - 1) || isEnemy(index, fromX + 2, fromY - 1)))
        possMoves << Cell { fromX + 2, fromY - 1 };

    if (isTrueCell(fromX + 2, fromY + 1) &&
            (isFreeCell(fromX + 2, fromY + 1) || isEnemy(index, fromX + 2, fromY + 1)))
        possMoves << Cell { fromX + 2, fromY + 1 };

    return possMoves;
}

QList<Logic::Cell> Logic::addMovesBishop(int index, int fromX, int fromY)
{
    QList<Cell> possMoves;
    int i;

    i = 0;
    while (++i && isTrueCell(fromX - i, fromY - i) && isFreeCell(fromX - i, fromY - i))
        possMoves << Cell { fromX - i, fromY - i };
    if (isTrueCell(fromX - i, fromY - i) && isEnemy(index, fromX - i, fromY - i))
        possMoves << Cell { fromX - i, fromY - i };

    i = 0;
    while (++i && isTrueCell(fromX - i, fromY + i) && isFreeCell(fromX - i, fromY + i))
        possMoves << Cell { fromX - i, fromY + i };
    if (isTrueCell(fromX - i, fromY + i) && isEnemy(index, fromX - i, fromY + i))
        possMoves << Cell { fromX - i, fromY + i };

    i = 0;
    while (++i && isTrueCell(fromX + i, fromY - i) && isFreeCell(fromX + i, fromY - i))
        possMoves << Cell { fromX + i, fromY - i };
    if (isTrueCell(fromX + i, fromY - i) && isEnemy(index, fromX + i, fromY - i))
        possMoves << Cell { fromX + i, fromY - i };

    i = 0;
    while (++i && isTrueCell(fromX + i, fromY + i) && isFreeCell(fromX + i, fromY + i))
        possMoves << Cell { fromX + i, fromY + i };
    if (isTrueCell(fromX + i, fromY + i) && isEnemy(index, fromX + i, fromY + i))
        possMoves << Cell { fromX + i, fromY + i };

    return possMoves;
}

QList<Logic::Cell> Logic::addMovesQueen(int index, int fromX, int fromY)
{
    QList<Cell> possMoves;

    possMoves = addMovesRook(index, fromX, fromY) +
            addMovesBishop(index, fromX, fromY);

    return possMoves;
}

QList<Logic::Cell> Logic::addMovesKing(int index, int fromX, int fromY)
{
    QList<Cell> possMoves;

    for (int i = -1; i <= 1; ++i )
        for (int j = -1; j <= 1; ++j)
            if (isTrueCell(fromX + i, fromY + j) &&
                    (isFreeCell(fromX + i, fromY + j) || isEnemy(index, fromX + i, fromY + j)))
                possMoves << Cell { fromX + i, fromY + j };

    return possMoves;
}

QList<Logic::Cell> Logic::addMovesPawn(int index, int fromX, int fromY)
{
    QList<Cell> possMoves;
    int i;

    i = isWhiteFigure(impl->figures[index].type) ? -1 : 1;
    if (isTrueCell(fromX, fromY + i) && isFreeCell(fromX, fromY + i))
    {
        possMoves << Cell { fromX, fromY + i };
        if ((fromY == isWhiteFigure(impl->figures[index].type) ? BOARD_SIZE - 2 : 1) &&
                isTrueCell(fromX, fromY + i + i) && isFreeCell(fromX, fromY + i + i))
            possMoves << Cell { fromX, fromY + i + i };
    }
    if (isTrueCell(fromX - 1, fromY + i) && isEnemy(index, fromX - 1, fromY + i))
        possMoves << Cell { fromX - 1, fromY + i };
    if (isTrueCell(fromX + 1, fromY + i) && isEnemy(index, fromX + 1, fromY + i))
        possMoves << Cell { fromX + 1, fromY + i };

    return possMoves;
}

void Logic::addMoveToHistory(int index, int fromX, int fromY, int toX, int toY)
{
    history << Move { index, fromX, fromY, toX, toY };
    ++indexHistory;
}

int Logic::getCurrentTurn()
{
    return currentTurn;
}

bool Logic::isCorrectTurn(int type)
{
    return (isWhiteFigure(type) && currentTurn == 0) ||
            (!isWhiteFigure(type) && currentTurn == 1);
}

bool Logic::isDeadKingWhite()
{
    return impl->figures[INDEX_KING_WHITE].x == BOARD_SIZE &&
            impl->figures[INDEX_KING_WHITE].y == BOARD_SIZE;
}

bool Logic::isDeadKingBlack()
{
    return impl->figures[INDEX_KING_BLACK].x == BOARD_SIZE &&
            impl->figures[INDEX_KING_BLACK].y == BOARD_SIZE;
}

void Logic::saveHistory()
{
    QFile file(QDir::currentPath() + "/src/SavedGame");

    if (file.open(QIODevice::WriteOnly))
    {
        QTextStream out(&file);
        for (int i = 0; i < NUMBER_FIGURES; ++i)
            out << i << " " << impl->figures[i].x << " " << impl->figures[i].y << endl;
        out << endl;
        for (auto it = history.begin(); it != history.end(); ++it)
            out << it->index << " " << it->fromX << " " << it->fromY << " " <<
                   it->toX << " " << it->toY << endl;
        file.close();
    }
}

void Logic::newGame()
{
    if (!history.isEmpty())
        history.clear();
    currentTurn = 0;
    setStartFigures();
    QModelIndex topLeft = createIndex(0, 0);
    QModelIndex bottomRight = createIndex(NUMBER_FIGURES - 1, 0);
    emit dataChanged(topLeft, bottomRight);
}

void Logic::loadGame()
{
    QFile file(QDir::currentPath() + "/src/SavedGame");

    if (file.open(QIODevice::ReadOnly))
    {
        QTextStream in(&file);
        int index = 0;

        while (!in.atEnd() && index != NUMBER_FIGURES - 1)
        {
            int x, y;
            in >> index >> x >> y;
            impl->figures[index].x = x;
            impl->figures[index].y = y;
        }
        if (impl->figures.size() != NUMBER_FIGURES)
            return ;
        if (!history.isEmpty())
            history.clear();
        while (!in.atEnd())
        {
            int fromX, fromY, toX, toY;
            in >> index >> fromX >> fromY >> toX >> toY;
            if (!index && !fromX && !fromY && !toX && !toY)
                break;
            history << Move { index, fromX, fromY, toX, toY };
        }
        file.close();

        if (history.isEmpty())
            return ;
        indexHistory = history.size() - 1;
        index = history[indexHistory].index;
        currentTurn = isWhiteFigure(impl->figures[index].type) ? 1 : 0;

        QModelIndex topLeft = createIndex(0, 0);
        QModelIndex bottomRight = createIndex(NUMBER_FIGURES - 1, 0) ;
        emit dataChanged(topLeft, bottomRight);
    }
}

int Logic::getIndexHistory()
{
    return indexHistory;
}

void Logic::prevMove()
{
    int index;

    index = history[indexHistory].index;
    impl->figures[index].x = history[indexHistory].fromX;
    impl->figures[index].y = history[indexHistory].fromY;
    currentTurn = isWhiteFigure(impl->figures[index].type) ? 0 : 1;
    --indexHistory;
    if (indexHistory > -1 &&
            history[indexHistory].toX == BOARD_SIZE &&
            history[indexHistory].toY == BOARD_SIZE)
    {
        index = history[indexHistory].index;
        impl->figures[index].x = history[indexHistory].fromX;
        impl->figures[index].y = history[indexHistory].fromY;
        --indexHistory;
    }
    QModelIndex topLeft = createIndex(0, 0);
    QModelIndex bottomRight = createIndex(NUMBER_FIGURES - 1, 0);
    emit dataChanged(topLeft, bottomRight);
}

void Logic::nextMove()
{
    int index;

    ++indexHistory;
    if (history[indexHistory].toX == BOARD_SIZE &&
            history[indexHistory].toY == BOARD_SIZE)
    {
        index = history[indexHistory].index;
        impl->figures[index].x = history[indexHistory].toX;
        impl->figures[index].y = history[indexHistory].toY;
        ++indexHistory;
    }
    index = history[indexHistory].index;
    impl->figures[index].x = history[indexHistory].toX;
    impl->figures[index].y = history[indexHistory].toY;
    currentTurn = isWhiteFigure(impl->figures[index].type) ? 1 : 0;
    QModelIndex topLeft = createIndex(0, 0);
    QModelIndex bottomRight = createIndex(NUMBER_FIGURES - 1, 0);
    emit dataChanged(topLeft, bottomRight);
}

bool Logic::isNewestMove()
{
    return indexHistory == history.size() - 1;
}

void Logic::eraseHistory()
{
    if (!history.isEmpty())
        while (indexHistory < history.size() - 1)
            history.removeLast();
}

bool Logic::isCorrectSavedGame()
{
    QFile file(QDir::currentPath() + "/src/SavedGame");
    if (!file.exists())
        return false;
    return true;
}
