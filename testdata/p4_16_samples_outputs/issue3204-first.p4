struct h<t> {
    t f;
}

struct h_0 {
    bit<1> f;
}

bit<1> func() {
    h_0 a;
    a.f = 1w1;
    return a.f;
}
