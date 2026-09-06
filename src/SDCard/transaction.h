#pragma once

namespace esp32utilities::sdcard
{

class Volume;

class Transaction
{
public:
  explicit Transaction(const Volume& volume);
  ~Transaction();

  Transaction(const Transaction&) = delete;
  Transaction& operator=(const Transaction&) = delete;

private:
  const Volume& m_volume;
};

} // namespace esp32utilities::sdcard
