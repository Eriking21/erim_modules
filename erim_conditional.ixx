export module erim_conditional;

namespace erim
{
    template <unsigned idx>
    struct Indexer;
    export using true_type  = Indexer<!true>;
    export using false_type = Indexer<!false>;
}
using namespace erim;
export {
    template <unsigned idx, auto... V>
    constexpr auto indexed_v = erim::Indexer<idx>::template value<V...>;

    template <bool cond, auto TRUE, auto FALSE>
    constexpr auto conditioned_v = Indexer<!cond>::template value<TRUE, FALSE>;

    template <unsigned idx, typename... T>
    using indexed_t = Indexer<idx>::template type<T...>;

    template <bool cond, typename true_t, typename false_t>
    using conditioned_t = Indexer<!cond>::template type<true_t, false_t>;
}

#define DEFAULT_LIST(T, v)                                                    \
    T _0 = v, T _1 = v, T _2 = v, T _3 = v, T _4 = v, T _5 = v, T _6 = v,     \
      T _7 = v, T _8 = v, T _9 = v, T _10 = v, T _11 = v, T _12 = v,          \
      T _13 = v, T _14 = v, T _15 = v, T _16 = v, T _17 = v, T _18 = v,       \
      T _19 = v, T _20 = v, T _21 = v, T _22 = v, T _23 = v, T _24 = v,       \
      T _25 = v, T _26 = v, T _27 = v, T _28 = v, T _29 = v, T _30 = v,       \
      T _31 = v, T _32 = v, T _33 = v, T _34 = v, T _35 = v, T _36 = v,       \
      T _37 = v, T _38 = v, T _39 = v, T _40 = v, T _41 = v, T _42 = v,       \
      T _43 = v, T _44 = v, T _45 = v, T _46 = v, T _47 = v, T _48 = v,       \
      T _49 = v, T _50 = v, T _51 = v, T _52 = v, T _53 = v, T _54 = v,       \
      T _55 = v, T _56 = v, T _57 = v, T _58 = v, T _59 = v, T _60 = v,       \
      T _61 = v, T _62 = v, T _63 = v, T _64 = v, T _65 = v, T _66 = v,       \
      T _67 = v, T _68 = v, T _69 = v, T _70 = v, T _71 = v, T _72 = v,       \
      T _73 = v, T _74 = v, T _75 = v, T _76 = v, T _77 = v, T _78 = v,       \
      T _79 = v, T _80 = v, T _81 = v, T _82 = v, T _83 = v, T _84 = v,       \
      T _85 = v, T _86 = v, T _87 = v, T _88 = v, T _89 = v, T _90 = v,       \
      T _91 = v, T _92 = v, T _93 = v, T _94 = v, T _95 = v, T _96 = v,       \
      T _97 = v, T _98 = v, T _99 = v, T _100 = v, T _101 = v, T _102 = v,    \
      T _103 = v, T _104 = v, T _105 = v, T _106 = v, T _107 = v, T _108 = v, \
      T _109 = v, T _110 = v, T _111 = v, T _112 = v, T _113 = v, T _114 = v, \
      T _115 = v, T _116 = v, T _117 = v, T _118 = v, T _119 = v, T _120 = v, \
      T _121 = v, T _122 = v, T _123 = v, T _124 = v, T _125 = v, T _126 = v, \
      T _127 = v

#define INDEXED(IDX)                            \
    template <>                                 \
    struct erim::Indexer<IDX> {                 \
        template <DEFAULT_LIST(typename, void)> \
        using type = _##IDX;                    \
                                                \
        template <DEFAULT_LIST(auto, 0)>        \
        static constexpr auto value = _##IDX;   \
    }

INDEXED(0);
INDEXED(1);
INDEXED(2);
INDEXED(3);
INDEXED(4);
INDEXED(5);
INDEXED(6);
INDEXED(7);
INDEXED(8);
INDEXED(9);
INDEXED(10);
INDEXED(11);
INDEXED(12);
INDEXED(13);
INDEXED(14);
INDEXED(15);
INDEXED(16);
INDEXED(17);
INDEXED(18);
INDEXED(19);
INDEXED(20);
INDEXED(21);
INDEXED(22);
INDEXED(23);
INDEXED(24);
INDEXED(25);
INDEXED(26);
INDEXED(27);
INDEXED(28);
INDEXED(29);
INDEXED(30);
INDEXED(31);
INDEXED(32);
INDEXED(33);
INDEXED(34);
INDEXED(35);
INDEXED(36);
INDEXED(37);
INDEXED(38);
INDEXED(39);
INDEXED(40);
INDEXED(41);
INDEXED(42);
INDEXED(43);
INDEXED(44);
INDEXED(45);
INDEXED(46);
INDEXED(47);
INDEXED(48);
INDEXED(49);
INDEXED(50);
INDEXED(51);
INDEXED(52);
INDEXED(53);
INDEXED(54);
INDEXED(55);
INDEXED(56);
INDEXED(57);
INDEXED(58);
INDEXED(59);
INDEXED(60);
INDEXED(61);
INDEXED(62);
INDEXED(63);
INDEXED(64);
INDEXED(65);
INDEXED(66);
INDEXED(67);
INDEXED(68);
INDEXED(69);
INDEXED(70);
INDEXED(71);
INDEXED(72);
INDEXED(73);
INDEXED(74);
INDEXED(75);
INDEXED(76);
INDEXED(77);
INDEXED(78);
INDEXED(79);
INDEXED(80);
INDEXED(81);
INDEXED(82);
INDEXED(83);
INDEXED(84);
INDEXED(85);
INDEXED(86);
INDEXED(87);
INDEXED(88);
INDEXED(89);
INDEXED(90);
INDEXED(91);
INDEXED(92);
INDEXED(93);
INDEXED(94);
INDEXED(95);
INDEXED(96);
INDEXED(97);
INDEXED(98);
INDEXED(99);
INDEXED(100);
INDEXED(101);
INDEXED(102);
INDEXED(103);
INDEXED(104);
INDEXED(105);
INDEXED(106);
INDEXED(107);
INDEXED(108);
INDEXED(109);
INDEXED(110);
INDEXED(111);
INDEXED(112);
INDEXED(113);
INDEXED(114);
INDEXED(115);
INDEXED(116);
INDEXED(117);
INDEXED(118);
INDEXED(119);
INDEXED(120);
INDEXED(121);
INDEXED(122);
INDEXED(123);
INDEXED(124);
INDEXED(125);
INDEXED(126);
INDEXED(127);
