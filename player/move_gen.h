// Copyright (c) 2015 MIT License by 6.172 Staff

#ifndef MOVE_GEN_H
#define MOVE_GEN_H

#include <inttypes.h>
#include <stdbool.h>
#include <stddef.h>
#include "./tbassert.h"
#include "./util.h"

// The MAX_NUM_MOVES is just an estimate
#define MAX_NUM_MOVES 1024      // real number = 8 * (3 + 8 + 8 * (7 + 3)) = 728
#define MAX_PLY_IN_SEARCH 100  // up to 100 ply
#define MAX_PLY_IN_GAME 4096   // long game!  ;^)

// Used for debugging and display
#define MAX_CHARS_IN_MOVE 16  // Could be less
#define MAX_CHARS_IN_TOKEN 64

// -----------------------------------------------------------------------------
// Board
// -----------------------------------------------------------------------------

// The board (which is 8x8 or 10x10) is centered in a 16x16 array, with the
// excess height and width being used for sentinels.
#define ARR_WIDTH 10
#define ARR_SIZE (ARR_WIDTH * ARR_WIDTH)

// Board is 8 x 8 or 10 x 10
#define BOARD_WIDTH 8

typedef int square_t;
typedef int rnk_t;
typedef int fil_t;

#define FIL_ORIGIN ((ARR_WIDTH - BOARD_WIDTH) / 2)
#define RNK_ORIGIN ((ARR_WIDTH - BOARD_WIDTH) / 2)

#define FIL_SHIFT 4
#define FIL_MASK 15
#define RNK_SHIFT 0
#define RNK_MASK 15

static fil_t SQR_FIL[ARR_SIZE] = {
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
  6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
  7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
  8, 8, 8, 8, 8, 8, 8, 8, 8, 8
};

static rnk_t SQR_RNK[ARR_SIZE] = {
  -1, 0, 1, 2, 3, 4, 5, 6, 7, 8,
  -1, 0, 1, 2, 3, 4, 5, 6, 7, 8,
  -1, 0, 1, 2, 3, 4, 5, 6, 7, 8,
  -1, 0, 1, 2, 3, 4, 5, 6, 7, 8,
  -1, 0, 1, 2, 3, 4, 5, 6, 7, 8,
  -1, 0, 1, 2, 3, 4, 5, 6, 7, 8,
  -1, 0, 1, 2, 3, 4, 5, 6, 7, 8,
  -1, 0, 1, 2, 3, 4, 5, 6, 7, 8,
  -1, 0, 1, 2, 3, 4, 5, 6, 7, 8,
  -1, 0, 1, 2, 3, 4, 5, 6, 7, 8
};

// -----------------------------------------------------------------------------
// Pieces
// -----------------------------------------------------------------------------

#define PIECE_SIZE 5  // Number of bits in (ptype, color, orientation)

typedef uint8_t piece_t;

// -----------------------------------------------------------------------------
// Piece types
// -----------------------------------------------------------------------------

#define PTYPE_SHIFT 2
#define PTYPE_MASK 3

typedef enum {
  EMPTY,
  PAWN,
  KING,
  INVALID
} ptype_t;

// -----------------------------------------------------------------------------
// Colors
// -----------------------------------------------------------------------------

#define COLOR_SHIFT 4
#define COLOR_MASK 1

typedef enum {
  WHITE = 0,
  BLACK
} color_t;

// -----------------------------------------------------------------------------
// Orientations
// -----------------------------------------------------------------------------

#define NUM_ORI 4
#define ORI_SHIFT 0
#define ORI_MASK (NUM_ORI - 1)

typedef enum {
  NN,
  EE,
  SS,
  WW
} king_ori_t;

typedef enum {
  NW,
  NE,
  SE,
  SW
} pawn_ori_t;

// -----------------------------------------------------------------------------
// Moves
// -----------------------------------------------------------------------------

// MOVE_MASK is 28 bits
// Representation is PTYPE_MV|ROT|FROM|INTERMEDIATE|TO
// INTERMEDIATE is equal to FROM iff the move is not a double move
#define MOVE_MASK 0xfffffff

#define PTYPE_MV_SHIFT 26
#define PTYPE_MV_MASK 3
#define FROM_SHIFT 16
#define FROM_MASK 0xFF
#define INTERMEDIATE_SHIFT 8
#define INTERMEDIATE_MASK 0xFF
#define TO_SHIFT 0
#define TO_MASK 0xFF
#define ROT_SHIFT 24
#define ROT_MASK 3

typedef uint32_t move_t;
typedef uint64_t sortable_move_t;  // extra bits used to store sort key

// Rotations
typedef enum {
  NONE,
  RIGHT,
  UTURN,
  LEFT
} rot_t;

// A single move should only be able to zap 1 piece now.
typedef struct victims_t {
  int zapped_count;
  piece_t zapped;
} victims_t;

// returned by make move in illegal situation
#define KO_ZAPPED -1
// returned by make move in ko situation
#define ILLEGAL_ZAPPED -1

// -----------------------------------------------------------------------------
// Position
// -----------------------------------------------------------------------------

// Board representation is square-centric with sentinels.
//
// https://www.chessprogramming.org/Board_Representation
// https://www.chessprogramming.org/Mailbox
// https://www.chessprogramming.org/10x12_Board

typedef struct position {
  piece_t      board[ARR_SIZE];
  struct position*  history;     // history of position
  uint64_t     key;              // hash key
  int          ply;              // Even ply are White, odd are Black
  move_t       last_move;        // move that led to this position
  victims_t    victims;          // pieces destroyed by shooter
  square_t     kloc[2];          // location of kings
} position_t;

// -----------------------------------------------------------------------------
// Function prototypes
// -----------------------------------------------------------------------------

const char* color_to_str(color_t c);
// Original prototypes before static inlining
// color_t color_of(piece_t x);
// color_t opp_color(color_t c);
// void set_color(piece_t* x, color_t c);
// ptype_t ptype_of(piece_t x);
// void set_ptype(piece_t* x, ptype_t pt);
// int ori_of(piece_t x);
// void set_ori(piece_t* x, int ori);

// which color is moving next
static inline color_t color_to_move_of(position_t* p) {
  if ((p->ply & 1) == 0) {
    return WHITE;
  } else {
    return BLACK;
  }
}

static inline color_t color_of(piece_t x) {
  return (color_t)((x >> COLOR_SHIFT) & COLOR_MASK);
}

static inline color_t opp_color(color_t c) {
  if (c == WHITE) {
    return BLACK;
  } else {
    return WHITE;
  }
}

static inline void set_color(piece_t* x, color_t c) {
  tbassert((c >= 0) & (c <= COLOR_MASK), "color: %d\n", c);
  *x = ((c & COLOR_MASK) << COLOR_SHIFT) |
       (*x & ~(COLOR_MASK << COLOR_SHIFT));
}

static inline ptype_t ptype_of(piece_t x) {
  return (ptype_t)((x >> PTYPE_SHIFT) & PTYPE_MASK);
}

static inline void set_ptype(piece_t* x, ptype_t pt) {
  *x = ((pt & PTYPE_MASK) << PTYPE_SHIFT) |
       (*x & ~(PTYPE_MASK << PTYPE_SHIFT));
}

static inline int ori_of(piece_t x) {
  return (x >> ORI_SHIFT) & ORI_MASK;
}

static inline void set_ori(piece_t* x, int ori) {
  *x = ((ori & ORI_MASK) << ORI_SHIFT) |
       (*x & ~(ORI_MASK << ORI_SHIFT));
}

void init_zob();
uint64_t compute_zob_key(position_t* p);

// Original prototypes before static inlining
// square_t square_of(fil_t f, rnk_t r);
// fil_t fil_of(square_t sq);
// rnk_t rnk_of(square_t sq);
// int square_to_str(square_t sq, char* buf, size_t bufsize);

// For no square, use 0, which is guaranteed to be off board
static inline square_t square_of(fil_t f, rnk_t r) {
  square_t s = ARR_WIDTH * (FIL_ORIGIN + f) + RNK_ORIGIN + r;
  DEBUG_LOG(1, "Square of (file %d, rank %d) is %d\n", f, r, s);
  tbassert((s >= 0) && (s < ARR_SIZE), "s: %d\n", s);
  return s;
}

// Finds file of square
static inline fil_t fil_of_reference(square_t sq) {
  fil_t f = ((sq >> FIL_SHIFT) & FIL_MASK) - FIL_ORIGIN;
  DEBUG_LOG(1, "File of square %d is %d\n", sq, f);
  return f;
}

// Finds file of square
static inline fil_t fil_of(square_t sq) {
  fil_t f = SQR_FIL[sq];
  DEBUG_LOG(1, "File of square %d is %d\n", sq, f);

  // N.B. This regression is incorrect for 10x10 board, only correct for 16x16 board
  // tbassert(((fil_t) f) == fil_of_reference(sq),
  //  "REGRESSION FAILURE! %d != %d !!\n", ((fil_t) f) ,fil_of_reference(sq));
  return f;
}

// Finds rank of square
static inline rnk_t rnk_of_reference(square_t sq) {
  rnk_t r = ((sq >> RNK_SHIFT) & RNK_MASK) - RNK_ORIGIN;
  DEBUG_LOG(1, "Rank of square %d is %d\n", sq, r);
  return r;
}

// Finds rank of square
static inline rnk_t rnk_of(square_t sq) {
  rnk_t r = SQR_RNK[sq];
  DEBUG_LOG(1, "Rank of square %d is %d\n", sq, r);
  // N.B. This regression is incorrect for 10x10 board, only correct for 16x16 board
  // tbassert(((rnk_t) r) == rnk_of_reference(sq),
  //  "REGRESSION FAILURE! %d != %d !!\n", ((rnk_t) r) ,rnk_of_reference(sq));
  return r;
}

// converts a square to string notation, returns number of characters printed
static inline int square_to_str(square_t sq, char* buf, size_t bufsize) {
  fil_t f = fil_of(sq);
  rnk_t r = rnk_of(sq);
  if (f >= 0) {
    return snprintf(buf, bufsize, "%c%d", 'a' + f, r);
  } else  {
    return snprintf(buf, bufsize, "%c%d", 'z' + f + 1, r);
  }
}

// Original prototypes before inlining
// int dir_of(int i);
// int beam_of(int direction);
// int reflect_of(int beam_dir, int pawn_ori);

// direction map
static int dir[8] = { -ARR_WIDTH - 1, -ARR_WIDTH, -ARR_WIDTH + 1, -1, 1,
                      ARR_WIDTH - 1, ARR_WIDTH, ARR_WIDTH + 1
                    };
static inline int dir_of(int i) {
  tbassert(i >= 0 && i < 8, "i: %d\n", i);
  return dir[i];
}


// directions for laser: NN, EE, SS, WW
static int beam[NUM_ORI] = {1, ARR_WIDTH, -1, -ARR_WIDTH};

static inline int beam_of(int direction) {
  tbassert(direction >= 0 && direction < NUM_ORI, "dir: %d\n", direction);
  return beam[direction];
}

// reflect[beam_dir][pawn_orientation]
// -1 indicates back of Pawn
static int reflect[NUM_ORI][NUM_ORI] = {
  //  NW  NE  SE  SW
  { -1, -1, EE, WW},   // NN
  { NN, -1, -1, SS},   // EE
  { WW, EE, -1, -1 },  // SS
  { -1, NN, SS, -1 }   // WW
};

static inline int reflect_of(int beam_dir, int pawn_ori) {
  tbassert(beam_dir >= 0 && beam_dir < NUM_ORI, "beam-dir: %d\n", beam_dir);
  tbassert(pawn_ori >= 0 && pawn_ori < NUM_ORI, "pawn-ori: %d\n", pawn_ori);
  return reflect[beam_dir][pawn_ori];
}

// Original prototypes
// ptype_t ptype_mv_of(move_t mv);
// square_t from_square(move_t mv);
// square_t intermediate_square(move_t mv);
// square_t to_square(move_t mv);
// rot_t rot_of(move_t mv);
// move_t move_of(ptype_t typ, rot_t rot, square_t from_sq,
//                square_t intermediate_sq, square_t to_sq);
// void move_to_str(move_t mv, char* buf, size_t bufsize);

static inline ptype_t ptype_mv_of(move_t mv) {
  return (ptype_t)((mv >> PTYPE_MV_SHIFT) & PTYPE_MV_MASK);
}

static inline square_t from_square(move_t mv) {
  return (mv >> FROM_SHIFT) & FROM_MASK;
}

static inline square_t intermediate_square(move_t mv) {
  return (mv >> INTERMEDIATE_SHIFT) & INTERMEDIATE_MASK;
}

static inline square_t to_square(move_t mv) {
  return (mv >> TO_SHIFT) & TO_MASK;
}

static inline rot_t rot_of(move_t mv) {
  return (rot_t)((mv >> ROT_SHIFT) & ROT_MASK);
}

static inline move_t move_of(ptype_t typ, rot_t rot, square_t from_sq, square_t int_sq, square_t to_sq) {
  return ((typ & PTYPE_MV_MASK) << PTYPE_MV_SHIFT) |
         ((rot & ROT_MASK) << ROT_SHIFT) |
         ((from_sq & FROM_MASK) << FROM_SHIFT) |
         ((int_sq & INTERMEDIATE_MASK) << INTERMEDIATE_SHIFT) |
         ((to_sq & TO_MASK) << TO_SHIFT);
}

// converts a move to string notation for FEN
static inline void move_to_str(move_t mv, char* buf, size_t bufsize) {
  square_t f = from_square(mv);  // from-square
  square_t i = intermediate_square(mv); //int-square
  square_t t = to_square(mv);    // to-square
  rot_t r = rot_of(mv);          // rotation
  const char* orig_buf = buf;

  buf += square_to_str(f, buf, bufsize);
  if (f != i) {
    buf += square_to_str(i, buf, bufsize - (buf - orig_buf));
  }
  if (i != t) {
    buf += square_to_str(t, buf, bufsize - (buf - orig_buf));
  }

  switch (r) {
    case NONE:
      break;
    case RIGHT:
      buf += snprintf(buf, bufsize - (buf - orig_buf), "R");
      break;
    case UTURN:
      buf += snprintf(buf, bufsize - (buf - orig_buf), "U");
      break;
    case LEFT:
      buf += snprintf(buf, bufsize - (buf - orig_buf), "L");
      break;
    default:
      tbassert(false, "Whoa, now.  Whoa, I say.\n");  // Bad, bad, bad
      break;
  }
}

int generate_all(position_t* p, sortable_move_t* sortable_move_list,
                 bool strict);
int generate_all_with_color(position_t* p, sortable_move_t* sortable_move_list, color_t color_to_move);
void do_perft(position_t* gme, int depth, int ply);
void low_level_make_move(position_t* old, position_t* p, move_t mv);
victims_t make_move(position_t* old, position_t* p, move_t mv);
void display(position_t* p);

// -----------------------------------------------------------------------------
// Ko and illegal move signalling
// -----------------------------------------------------------------------------

// Original prototypes before static inlining
// victims_t KO();
// victims_t ILLEGAL();

// bool is_ILLEGAL(victims_t victims);
// bool is_KO(victims_t victims);
// bool zero_victims(victims_t victims);
// bool victim_exists(victims_t victims);

static inline victims_t KO() {
  return ((victims_t) {
    KO_ZAPPED, 0
  });
}

static inline victims_t ILLEGAL() {
  return ((victims_t) {
    ILLEGAL_ZAPPED, 0
  });
}

static inline bool is_KO(victims_t victims) {
  return (victims.zapped_count == KO_ZAPPED);
}

static inline bool is_ILLEGAL(victims_t victims) {
  return (victims.zapped_count == ILLEGAL_ZAPPED);
}

static inline bool zero_victims(victims_t victims) {
  return (victims.zapped_count == 0);
}

static inline bool victim_exists(victims_t victims) {
  return (victims.zapped_count > 0);
}

#endif  // MOVE_GEN_H
