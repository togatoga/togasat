// Tests for namespace-level constants and basic types
// Note: Lit struct is private, so we test through public API

TEST_SUITE("Namespace Constants") {
  TEST_CASE("lbool constants are defined correctly") {
    CHECK(togasat::l_True == 0);
    CHECK(togasat::l_False == 1);
    CHECK(togasat::l_Undef == 2);
  }

  TEST_CASE("CRef_Undef is defined correctly") {
    CHECK(togasat::CRef_Undef == -1);
  }
}
