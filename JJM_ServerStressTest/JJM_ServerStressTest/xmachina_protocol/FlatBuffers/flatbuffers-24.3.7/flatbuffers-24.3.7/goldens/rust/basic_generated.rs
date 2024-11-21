// automatically generated by the FlatBuffers compiler, do not modify


// @generated

use core::mem;
use core::cmp::Ordering;

extern crate flatbuffers;
use self::flatbuffers::{EndianScalar, Follow};

#[allow(unused_imports, dead_code)]
pub mod flatbuffers {

  use core::mem;
  use core::cmp::Ordering;

  extern crate flatbuffers;
  use self::flatbuffers::{EndianScalar, Follow};
#[allow(unused_imports, dead_code)]
pub mod goldens {

  use core::mem;
  use core::cmp::Ordering;

  extern crate flatbuffers;
  use self::flatbuffers::{EndianScalar, Follow};

pub enum GalaxyOffset {}
#[derive(Copy, Clone, PartialEq)]

pub struct Galaxy<'a> {
  pub _tab: flatbuffers::Table<'a>,
}

impl<'a> flatbuffers::Follow<'a> for Galaxy<'a> {
  type Inner = Galaxy<'a>;
  #[inline]
  unsafe fn follow(buf: &'a [u8], loc: usize) -> Self::Inner {
    Self { _tab: flatbuffers::Table::new(buf, loc) }
  }
}

impl<'a> Galaxy<'a> {
  pub const VT_NUM_STARS: flatbuffers::VOffsetT = 4;

  #[inline]
  pub unsafe fn init_from_table(table: flatbuffers::Table<'a>) -> Self {
    Galaxy { _tab: table }
  }
  #[allow(unused_mut)]
  pub fn create<'bldr: 'args, 'args: 'mut_bldr, 'mut_bldr, A: flatbuffers::Allocator + 'bldr>(
    _fbb: &'mut_bldr mut flatbuffers::FlatBufferBuilder<'bldr, A>,
    args: &'args GalaxyArgs
  ) -> flatbuffers::WIPOffset<Galaxy<'bldr>> {
    let mut builder = GalaxyBuilder::new(_fbb);
    builder.add_num_stars(args.num_stars);
    builder.finish()
  }


  #[inline]
  pub fn num_stars(&self) -> i64 {
    // Safety:
    // Created from valid Table for this object
    // which contains a valid value in this slot
    unsafe { self._tab.get::<i64>(Galaxy::VT_NUM_STARS, Some(0)).unwrap()}
  }
}

impl flatbuffers::Verifiable for Galaxy<'_> {
  #[inline]
  fn run_verifier(
    v: &mut flatbuffers::Verifier, pos: usize
  ) -> Result<(), flatbuffers::InvalidFlatbuffer> {
    use self::flatbuffers::Verifiable;
    v.visit_table(pos)?
     .visit_field::<i64>("num_stars", Self::VT_NUM_STARS, false)?
     .finish();
    Ok(())
  }
}
pub struct GalaxyArgs {
    pub num_stars: i64,
}
impl<'a> Default for GalaxyArgs {
  #[inline]
  fn default() -> Self {
    GalaxyArgs {
      num_stars: 0,
    }
  }
}

pub struct GalaxyBuilder<'a: 'b, 'b, A: flatbuffers::Allocator + 'a> {
  fbb_: &'b mut flatbuffers::FlatBufferBuilder<'a, A>,
  start_: flatbuffers::WIPOffset<flatbuffers::TableUnfinishedWIPOffset>,
}
impl<'a: 'b, 'b, A: flatbuffers::Allocator + 'a> GalaxyBuilder<'a, 'b, A> {
  #[inline]
  pub fn add_num_stars(&mut self, num_stars: i64) {
    self.fbb_.push_slot::<i64>(Galaxy::VT_NUM_STARS, num_stars, 0);
  }
  #[inline]
  pub fn new(_fbb: &'b mut flatbuffers::FlatBufferBuilder<'a, A>) -> GalaxyBuilder<'a, 'b, A> {
    let start = _fbb.start_table();
    GalaxyBuilder {
      fbb_: _fbb,
      start_: start,
    }
  }
  #[inline]
  pub fn finish(self) -> flatbuffers::WIPOffset<Galaxy<'a>> {
    let o = self.fbb_.end_table(self.start_);
    flatbuffers::WIPOffset::new(o.value())
  }
}

impl core::fmt::Debug for Galaxy<'_> {
  fn fmt(&self, f: &mut core::fmt::Formatter<'_>) -> core::fmt::Result {
    let mut ds = f.debug_struct("Galaxy");
      ds.field("num_stars", &self.num_stars());
      ds.finish()
  }
}
pub enum UniverseOffset {}
#[derive(Copy, Clone, PartialEq)]

pub struct Universe<'a> {
  pub _tab: flatbuffers::Table<'a>,
}

impl<'a> flatbuffers::Follow<'a> for Universe<'a> {
  type Inner = Universe<'a>;
  #[inline]
  unsafe fn follow(buf: &'a [u8], loc: usize) -> Self::Inner {
    Self { _tab: flatbuffers::Table::new(buf, loc) }
  }
}

impl<'a> Universe<'a> {
  pub const VT_AGE: flatbuffers::VOffsetT = 4;
  pub const VT_GALAXIES: flatbuffers::VOffsetT = 6;

  #[inline]
  pub unsafe fn init_from_table(table: flatbuffers::Table<'a>) -> Self {
    Universe { _tab: table }
  }
  #[allow(unused_mut)]
  pub fn create<'bldr: 'args, 'args: 'mut_bldr, 'mut_bldr, A: flatbuffers::Allocator + 'bldr>(
    _fbb: &'mut_bldr mut flatbuffers::FlatBufferBuilder<'bldr, A>,
    args: &'args UniverseArgs<'args>
  ) -> flatbuffers::WIPOffset<Universe<'bldr>> {
    let mut builder = UniverseBuilder::new(_fbb);
    builder.add_age(args.age);
    if let Some(x) = args.galaxies { builder.add_galaxies(x); }
    builder.finish()
  }


  #[inline]
  pub fn age(&self) -> f64 {
    // Safety:
    // Created from valid Table for this object
    // which contains a valid value in this slot
    unsafe { self._tab.get::<f64>(Universe::VT_AGE, Some(0.0)).unwrap()}
  }
  #[inline]
  pub fn galaxies(&self) -> Option<flatbuffers::Vector<'a, flatbuffers::ForwardsUOffset<Galaxy<'a>>>> {
    // Safety:
    // Created from valid Table for this object
    // which contains a valid value in this slot
    unsafe { self._tab.get::<flatbuffers::ForwardsUOffset<flatbuffers::Vector<'a, flatbuffers::ForwardsUOffset<Galaxy>>>>(Universe::VT_GALAXIES, None)}
  }
}

impl flatbuffers::Verifiable for Universe<'_> {
  #[inline]
  fn run_verifier(
    v: &mut flatbuffers::Verifier, pos: usize
  ) -> Result<(), flatbuffers::InvalidFlatbuffer> {
    use self::flatbuffers::Verifiable;
    v.visit_table(pos)?
     .visit_field::<f64>("age", Self::VT_AGE, false)?
     .visit_field::<flatbuffers::ForwardsUOffset<flatbuffers::Vector<'_, flatbuffers::ForwardsUOffset<Galaxy>>>>("galaxies", Self::VT_GALAXIES, false)?
     .finish();
    Ok(())
  }
}
pub struct UniverseArgs<'a> {
    pub age: f64,
    pub galaxies: Option<flatbuffers::WIPOffset<flatbuffers::Vector<'a, flatbuffers::ForwardsUOffset<Galaxy<'a>>>>>,
}
impl<'a> Default for UniverseArgs<'a> {
  #[inline]
  fn default() -> Self {
    UniverseArgs {
      age: 0.0,
      galaxies: None,
    }
  }
}

pub struct UniverseBuilder<'a: 'b, 'b, A: flatbuffers::Allocator + 'a> {
  fbb_: &'b mut flatbuffers::FlatBufferBuilder<'a, A>,
  start_: flatbuffers::WIPOffset<flatbuffers::TableUnfinishedWIPOffset>,
}
impl<'a: 'b, 'b, A: flatbuffers::Allocator + 'a> UniverseBuilder<'a, 'b, A> {
  #[inline]
  pub fn add_age(&mut self, age: f64) {
    self.fbb_.push_slot::<f64>(Universe::VT_AGE, age, 0.0);
  }
  #[inline]
  pub fn add_galaxies(&mut self, galaxies: flatbuffers::WIPOffset<flatbuffers::Vector<'b , flatbuffers::ForwardsUOffset<Galaxy<'b >>>>) {
    self.fbb_.push_slot_always::<flatbuffers::WIPOffset<_>>(Universe::VT_GALAXIES, galaxies);
  }
  #[inline]
  pub fn new(_fbb: &'b mut flatbuffers::FlatBufferBuilder<'a, A>) -> UniverseBuilder<'a, 'b, A> {
    let start = _fbb.start_table();
    UniverseBuilder {
      fbb_: _fbb,
      start_: start,
    }
  }
  #[inline]
  pub fn finish(self) -> flatbuffers::WIPOffset<Universe<'a>> {
    let o = self.fbb_.end_table(self.start_);
    flatbuffers::WIPOffset::new(o.value())
  }
}

impl core::fmt::Debug for Universe<'_> {
  fn fmt(&self, f: &mut core::fmt::Formatter<'_>) -> core::fmt::Result {
    let mut ds = f.debug_struct("Universe");
      ds.field("age", &self.age());
      ds.field("galaxies", &self.galaxies());
      ds.finish()
  }
}
#[inline]
/// Verifies that a buffer of bytes contains a `Universe`
/// and returns it.
/// Note that verification is still experimental and may not
/// catch every error, or be maximally performant. For the
/// previous, unchecked, behavior use
/// `root_as_universe_unchecked`.
pub fn root_as_universe(buf: &[u8]) -> Result<Universe, flatbuffers::InvalidFlatbuffer> {
  flatbuffers::root::<Universe>(buf)
}
#[inline]
/// Verifies that a buffer of bytes contains a size prefixed
/// `Universe` and returns it.
/// Note that verification is still experimental and may not
/// catch every error, or be maximally performant. For the
/// previous, unchecked, behavior use
/// `size_prefixed_root_as_universe_unchecked`.
pub fn size_prefixed_root_as_universe(buf: &[u8]) -> Result<Universe, flatbuffers::InvalidFlatbuffer> {
  flatbuffers::size_prefixed_root::<Universe>(buf)
}
#[inline]
/// Verifies, with the given options, that a buffer of bytes
/// contains a `Universe` and returns it.
/// Note that verification is still experimental and may not
/// catch every error, or be maximally performant. For the
/// previous, unchecked, behavior use
/// `root_as_universe_unchecked`.
pub fn root_as_universe_with_opts<'b, 'o>(
  opts: &'o flatbuffers::VerifierOptions,
  buf: &'b [u8],
) -> Result<Universe<'b>, flatbuffers::InvalidFlatbuffer> {
  flatbuffers::root_with_opts::<Universe<'b>>(opts, buf)
}
#[inline]
/// Verifies, with the given verifier options, that a buffer of
/// bytes contains a size prefixed `Universe` and returns
/// it. Note that verification is still experimental and may not
/// catch every error, or be maximally performant. For the
/// previous, unchecked, behavior use
/// `root_as_universe_unchecked`.
pub fn size_prefixed_root_as_universe_with_opts<'b, 'o>(
  opts: &'o flatbuffers::VerifierOptions,
  buf: &'b [u8],
) -> Result<Universe<'b>, flatbuffers::InvalidFlatbuffer> {
  flatbuffers::size_prefixed_root_with_opts::<Universe<'b>>(opts, buf)
}
#[inline]
/// Assumes, without verification, that a buffer of bytes contains a Universe and returns it.
/// # Safety
/// Callers must trust the given bytes do indeed contain a valid `Universe`.
pub unsafe fn root_as_universe_unchecked(buf: &[u8]) -> Universe {
  flatbuffers::root_unchecked::<Universe>(buf)
}
#[inline]
/// Assumes, without verification, that a buffer of bytes contains a size prefixed Universe and returns it.
/// # Safety
/// Callers must trust the given bytes do indeed contain a valid size prefixed `Universe`.
pub unsafe fn size_prefixed_root_as_universe_unchecked(buf: &[u8]) -> Universe {
  flatbuffers::size_prefixed_root_unchecked::<Universe>(buf)
}
#[inline]
pub fn finish_universe_buffer<'a, 'b, A: flatbuffers::Allocator + 'a>(
    fbb: &'b mut flatbuffers::FlatBufferBuilder<'a, A>,
    root: flatbuffers::WIPOffset<Universe<'a>>) {
  fbb.finish(root, None);
}

#[inline]
pub fn finish_size_prefixed_universe_buffer<'a, 'b, A: flatbuffers::Allocator + 'a>(fbb: &'b mut flatbuffers::FlatBufferBuilder<'a, A>, root: flatbuffers::WIPOffset<Universe<'a>>) {
  fbb.finish_size_prefixed(root, None);
}
}  // pub mod goldens
}  // pub mod flatbuffers

